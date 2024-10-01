import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Random;

class Snake extends JPanel implements ActionListener {
    static final int screenWidth = 960;
    static final int screenHeight = 540;
    static final int unitSize = 40;
    static final int gameUnits = (screenWidth * screenHeight) / unitSize;
    static int delay = 250;
    final int[] x = new int[gameUnits];
    final int[] y = new int[gameUnits];
    int bodyParts = 3;
    int applesEaten = 0;
    int appleX;
    int appleY;
    char direction = 'R'; // Initial direction
    boolean running = false;
    Timer timer;
    Random random;
    JButton restartButton;
    JLabel scoreboard;
    int score =0;

    Snake() {
        random = new Random();
        this.setPreferredSize(new Dimension(screenWidth, screenHeight));
        this.setFocusable(true);
        this.addKeyListener(new MyKeys());
        this.setLayout(null);
        startGame();
        restartButton = new JButton("Restart");
        scoreboard = new JLabel();
        restartButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                restartGame();
            }
        });
        restartButton.setVisible(false);
        this.add(restartButton,BorderLayout.CENTER);
        scoreboard.setFont(new Font("Ink Free", Font.BOLD, 30));
        scoreboard.setText(String.valueOf(score));
        scoreboard.setBounds(10, 10, 150, 30);
        scoreboard.setBackground(Color.WHITE);
        scoreboard.setForeground(Color.BLACK);
        scoreboard.setOpaque(true);
        this.add(scoreboard);
    }
    public void startGame() {
        newApple();
        running = true;
        timer = new Timer(delay, this);
        timer.start();
    }
    public void restartGame(){
        bodyParts=3;
        applesEaten=0;
        score=0;
        direction='R';
        running=true;
        for(int i=0; i<bodyParts; i++)
        {
            x[i]=0;
            y[i]=0;
        }
        newApple();
        timer.restart();
        this.setFocusable(true);
        this.requestFocusInWindow();
        scoreboard.setText(String.valueOf(score));
        repaint();
        restartButton.setVisible(false);

    }
    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        draw(g);
    }

    public void draw(Graphics g) {
        if (running) {
            g.setColor(Color.lightGray);
            for (int i = 0; i < screenWidth / unitSize; i++) {
                g.drawLine(i * unitSize, 0, i * unitSize, screenHeight);
                g.drawLine(0, i * unitSize, screenWidth, i * unitSize);
            }
            g.setColor(Color.RED);
            g.fillOval(appleX, appleY, unitSize, unitSize);
            for (int i = 0; i < bodyParts; i++) {
                g.setColor(Color.decode("#58ed53"));
                g.fillOval(x[i], y[i], unitSize, unitSize);
            }
        } else {
            GameOver(g);
        }
    }
    public boolean checkAppleOverlap()
    {
        for(int i=0; i<bodyParts; i++)
        {
            if(x[i]==appleX && y[i]==appleY)
            {
                return true;
            }
        }
        return false;
    }
    public void newApple() {
        do {
            appleX = random.nextInt((int) screenWidth / unitSize) * unitSize;
            appleY = random.nextInt((int) screenHeight / unitSize) * unitSize;
        }while (checkAppleOverlap());
    }
    public void move() {
        for (int i = bodyParts; i > 0; i--) {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
        }
        switch (direction) {
            case 'U':
                y[0] = y[0] - unitSize;
                break;
            case 'R':
                x[0] = x[0] + unitSize;
                break;
            case 'L':
                x[0] = x[0] - unitSize;
                break;
            case 'D':
                y[0] = y[0] + unitSize;
                break;
        }
    }

    public void checkApple() {
        if(x[0]==appleX && y[0]==appleY)
        {
            bodyParts++;
            applesEaten++;
            if(delay>60) {
                delay = delay - 1;
            }
            score=score+100;
            scoreboard.setText(String.valueOf(score));
            newApple();
        }
    }

    public void checkCollisions() {
        for (int i = bodyParts; i > 0; i--) {
            if ((x[0] == x[i]) && (y[0] == y[i])) {
                running = false;
            }
        }
        if (x[0] < 0 || y[0] < 0 || x[0] >= screenWidth || y[0] >= screenHeight) {
            running = false;
        }
        if (!running) {
            timer.stop();
        }
    }

    public void GameOver(Graphics g) {
        g.setColor(Color.RED);
        g.setFont(new Font("Ink Free",Font.BOLD,75));
        FontMetrics metrics = getFontMetrics(g.getFont());
        g.drawString("GAME OVER",(screenWidth-metrics.stringWidth("GAME OVER"))/2,screenHeight/2);
        int buttonWidth = 200;
        int buttonHeight = 50;
        restartButton.setBounds((screenWidth - buttonWidth) / 2, screenHeight / 2 + 50, buttonWidth, buttonHeight);
        restartButton.setVisible(true);
        this.add(restartButton);
    }
    @Override
    public void actionPerformed(ActionEvent e) {
        if (running) {
            move();
            checkApple();
            checkCollisions();
        }
        repaint();
    }

    class MyKeys extends KeyAdapter {
        @Override
        public void keyPressed(KeyEvent e) {
            switch (e.getKeyCode()) {
                case KeyEvent.VK_A:
                    if (direction != 'R') direction = 'L';
                    break;
                case KeyEvent.VK_S:
                    if (direction != 'U') direction = 'D';
                    break;
                case KeyEvent.VK_W:
                    if (direction != 'D') direction = 'U';
                    break;
                case KeyEvent.VK_D:
                    if (direction != 'L') direction = 'R';
                    break;
            }
        }
    }
}
