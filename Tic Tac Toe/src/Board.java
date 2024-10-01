import javax.swing.*;
import javax.swing.border.LineBorder;
import java.awt.*;
import java.awt.event.ActionListener;

class Board extends JFrame {
    private JPanel panel1;
    private JPanel title_panel;
    private JPanel button_panel;
    private JButton[] buttons;
    public Board() {
        title_panel = new JPanel();
        JPanel button_panel = new JPanel();
        buttons = new JButton[9];
        setTitle("Tic Tac Toe");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setSize(1920, 1080);
        setLocationRelativeTo(null);
        setLayout(new BorderLayout());
        title_panel.setLayout(new BorderLayout());
        button_panel.setLayout(new GridLayout(3,3));
        for(int i=0; i<9; i++)
        {
            buttons[i]=new JButton();
            buttons[i].setBackground(Color.white);
            buttons[i].setBorder(new LineBorder(Color.black,2));
            button_panel.add(buttons[i]);
        }
        add(title_panel,BorderLayout.NORTH);
        add(button_panel,BorderLayout.CENTER);
        setVisible(true);
    }
    public void addButtonListener(ActionListener Listener)
    {
        for(JButton button:buttons)
        {
            button.addActionListener(Listener);
        }
    }
    public JButton[] getButtons(){
        return buttons;
    }
}
