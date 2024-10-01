import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Random;
class TicTacToe implements ActionListener {
    private Board game;
    private boolean isPlayerOneTurn;
    private JButton[] buttons;
    public TicTacToe() {
        Board game=new Board();
        game.addButtonListener(this);
        isPlayerOneTurn=decideWhoStarts()==0;
        buttons=game.getButtons();
    }
    public int decideWhoStarts()
    {
        Random random = new Random();
        return random.nextInt(2-1)+1;
    }
    @Override
    public void actionPerformed(ActionEvent e) {
        JButton buttonClicked = (JButton) e.getSource();
        ImageIcon icon;
        if (isPlayerOneTurn)
        {
            icon=new ImageIcon("C:\\Marius\\Faculty\\Special\\Tic Tac Toe\\src\\o.png");
        }else{
            icon=new ImageIcon("C:\\Marius\\Faculty\\Special\\Tic Tac Toe\\src\\x.png");
        }
        Image img= icon.getImage();
        Image resized=img.getScaledInstance(buttonClicked.getWidth(),buttonClicked.getHeight(),Image.SCALE_DEFAULT);
        buttonClicked.setIcon(new ImageIcon(resized));
        buttonClicked.setHorizontalAlignment(SwingConstants.CENTER);
        buttonClicked.setVerticalAlignment(SwingConstants.CENTER);
        buttonClicked.setActionCommand(isPlayerOneTurn?"Player1":"Player2");
        buttonClicked.setEnabled(false);
        isPlayerOneTurn=!isPlayerOneTurn;
        if(checkGameFinished()){
            JOptionPane.showMessageDialog(null,(isPlayerOneTurn?"Player 2":"Player 1")+" wins!");
            resetGame();
        }
        else if(CheckForDraw())
        {
            JOptionPane.showMessageDialog(null,"Game ended in a draw");
            resetGame();
        }
    }
    public boolean checkGameFinished()
    {
        for(int i=0; i<3; i++)
        {
            if(buttons[i*3].getActionCommand().equals(buttons[i*3+1].getActionCommand())&&
                    buttons[i*3+1].getActionCommand().equals(buttons[i*3+2].getActionCommand()) &&
                    !buttons[i*3].getActionCommand().isEmpty())
            {
                return true;
            }
            if(buttons[i].getActionCommand().equals(buttons[i+3].getActionCommand())&&
                    buttons[i+3].getActionCommand().equals(buttons[i+6].getActionCommand()) &&
                    !buttons[i].getActionCommand().isEmpty())
            {
                return true;
            }
            if (buttons[0].getActionCommand().equals(buttons[4].getActionCommand()) &&
                    buttons[4].getActionCommand().equals(buttons[8].getActionCommand()) &&
                    !buttons[0].getActionCommand().isEmpty()) {
                return true;
            }
            if (buttons[2].getActionCommand().equals(buttons[4].getActionCommand()) &&
                    buttons[4].getActionCommand().equals(buttons[6].getActionCommand()) &&
                    !buttons[2].getActionCommand().isEmpty()) {
                return true;
            }
        }
        return false;
    }
    private boolean CheckForDraw()
    {
        for(JButton button:buttons)
        {
            if(button.getActionCommand().isEmpty())
            {
                return false;
            }
        }
        return true;
    }
    private void resetGame()
    {
        for(JButton button:buttons){
            button.setIcon(null);
            button.setActionCommand("");
            button.setEnabled(true);
        }
        isPlayerOneTurn=decideWhoStarts()==0;
    }
    public static void main(String[] args) {
        new TicTacToe();
    }
}



