import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;

class CalculatorGui extends JFrame {

    private final int rows = 7;
    private final int columns = 5;
    private JTextPane resultText;
    private JPanel resultPanel;
    private JPanel buttonsPanel;
    private JPanel mainPanel;
    private JButton[][] buttons;
    private String[] symbols;
    public CalculatorGui() {
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setBackground(Color.BLACK);
        setSize(396, 657);
        symbols=new String[]{"","π","e","C","⌫","x²","1/x","|x|","exp","mod","√x","(",")","n!","/","xª","7","8","9","×","10^x","4","5","6","-","log","1","2","3","+","ln","+/-","0",".","="};
        resultPanel = new JPanel();
        buttonsPanel = new JPanel(new GridLayout(rows, columns)); // Use GridLayout for buttons
        mainPanel = new JPanel(new BorderLayout());
        resultText = new JTextPane();
        Font font = new Font("Serif", Font.PLAIN, 18);
        resultText.setFont(font);
        setRightAlignment(resultText);
        buttons = new JButton[rows][columns];
        addButtons();
        resultText.setText("0");
        resultText.setCaretPosition(resultText.getDocument().getLength());
        resultText.setPreferredSize(new Dimension(380, 100));
        resultPanel.setPreferredSize(new Dimension(380, 100));
        resultPanel.add(resultText);
        mainPanel.add(resultPanel, BorderLayout.NORTH);
        mainPanel.add(buttonsPanel, BorderLayout.CENTER);
        add(mainPanel);
        setVisible(true);
    }
    private void setRightAlignment(JTextPane textPane) {
        StyledDocument doc = textPane.getStyledDocument();
        SimpleAttributeSet right = new SimpleAttributeSet();
        StyleConstants.setAlignment(right, StyleConstants.ALIGN_RIGHT);
        doc.setParagraphAttributes(0, doc.getLength(), right, false);
    }
    public void addButtons() {
        int index=0;
        for(int i=0; i<rows; i++)
        {
            for(int j=0; j<columns; j++)
            {
                if(index<symbols.length)
                {
                    buttons[i][j]=new JButton(symbols[index]);
                    buttonsPanel.add(buttons[i][j]);
                    index++;
                }else{
                    buttons[i][j] = new JButton("");
                    buttonsPanel.add(buttons[i][j]);
                }
            }
        }
    }
    public JButton[][] getButtons(){
        return buttons;
    }
    public JTextPane getResultText(){
        return resultText;
    }
    public int getRows(){
        return rows;
    }
    public int getColumns(){
        return columns;
    }
}
