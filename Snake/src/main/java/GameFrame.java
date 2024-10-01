import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.Random;

class GameFrame extends JFrame {
    public GameFrame() {
        Snake snake = new Snake();
        snake.setBackground(Color.decode("#055e02"));
        this.add(snake);
        this.setTitle("Snake");
        this.setDefaultCloseOperation(EXIT_ON_CLOSE);
        this.setLocationRelativeTo(null);
        this.setResizable(false);
        this.pack();
        this.setSize(960, 540);
        this.setVisible(true);
    }
}
