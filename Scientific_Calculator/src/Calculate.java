import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Calculate implements ActionListener {
    private JButton[][] buttons;
    private CalculatorGui gui;
    private JTextPane resultText;
    private final int rows;
    private StringBuilder currentInput;
    private final int columns;
    private double firstNumber;
    private double secondNumber;
    private String currentOperation;

    public Calculate() {
        gui = new CalculatorGui();
        buttons = gui.getButtons();
        resultText = gui.getResultText();
        rows = gui.getRows();
        columns = gui.getColumns();
        currentInput = new StringBuilder();
        firstNumber = 0;
        secondNumber = 0;
        currentOperation = ""; // Initialize with an empty string

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                buttons[i][j].addActionListener(this);
            }
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        JButton buttonClicked = (JButton) e.getSource();
        String buttonText = buttonClicked.getText();
        String regex = "π|e|C|x²|1/x|\\|x\\||exp|mod|√x|n!|\\/|xª|×|10\\^x|-|log|\\+|ln|\\+/-";
        System.out.println("Button clicked: " + buttonClicked.getText());
        if (buttonText.equals("C")) {
            firstNumber = 0;
            secondNumber = 0;
            currentInput.setLength(0);
            resultText.setText("");
            currentOperation = "";
            return;
        }
        if (buttonText.matches("[0-9]")) {
            currentInput.append(buttonText);
            resultText.setText(currentInput.toString());
        }
        if (buttonText.equals("=")) {
            if (!currentInput.isEmpty()) {
                secondNumber = Double.parseDouble(currentInput.toString());
                PerformOperation(currentOperation);  // Perform the stored operation
                currentInput.setLength(0);  // Clear input for next operation
            }
        }
        if (buttonText.matches(regex)) {
            if (!currentInput.isEmpty()) {
                firstNumber = Double.parseDouble(currentInput.toString());
                currentInput.setLength(0);
            }
            if (buttonText.equals("π") || buttonText.equals("e")) {
                if (firstNumber == 0) {
                    firstNumber = buttonText.equals("π") ? Math.PI : Math.E;
                } else {
                    secondNumber = buttonText.equals("π") ? Math.PI : Math.E;
                }
                changeText(buttonText.equals("π") ? Math.PI : Math.E);
                return;
            }
            if (buttonText.equals("x²")) {
                firstNumber = Math.pow(firstNumber, 2);
                changeText(firstNumber);
                return;
            }
            if(buttonText.equals("1/x"))
            {
                firstNumber=1/firstNumber;
                changeText(firstNumber);
                return;
            }
            if(buttonText.equals("|x|"))
            {
                firstNumber=Math.abs(firstNumber);
                changeText(firstNumber);
                return;
            }
            if(buttonText.equals("exp"))
            {
                firstNumber=Math.exp(firstNumber);
                changeText(Math.exp(firstNumber));
                return;
            }
            if(buttonText.equals("√x"))
            {
                if (firstNumber >= 0) {
                    firstNumber = Math.sqrt(firstNumber);
                    changeText(firstNumber);
                } else {
                    resultText.setText("Error: Invalid input");
                }
                return;
            }
            if(buttonText.equals("n!"))
            {
                if (firstNumber >= 0 && firstNumber == Math.floor(firstNumber)) {
                    firstNumber = factorial((int) firstNumber);
                    changeText(firstNumber);
                } else {
                    resultText.setText("Error: Invalid input for factorial");
                }
                return;
            }
            if(buttonText.equals("10^x"))
            {
                firstNumber=Math.pow(10,firstNumber);
                changeText(firstNumber);
                return;
            }
            if(buttonText.equals("ln"))
            {
                firstNumber=Math.log(firstNumber);
                changeText(firstNumber);
                return;
            }
            if (buttonText.equals("log"))
            {
                firstNumber=Math.log10(firstNumber);
                changeText(firstNumber);
                return;
            }
            if (buttonText.equals("+/-"))
            {
                firstNumber=0-firstNumber;
                changeText(firstNumber);
                return;
            }
            currentOperation = buttonText;
        }
    }

    private double factorial(int n) {
        if (n == 0 || n == 1) {
            return 1;
        }
        double result = 1;
        for (int i = 2; i <= n; i++) {
            result *= i;
        }
        return result;
    }

    public void PerformOperation(String operator) {
        double result = 0;
        switch (operator) {
            case "+":
                result = firstNumber + secondNumber;
                break;
            case "-":
                result = firstNumber - secondNumber;
                break;
            case "×":
                result = firstNumber * secondNumber;
                break;
            case "/":
                if (secondNumber != 0) {
                    result = firstNumber / secondNumber;
                } else {
                    resultText.setText("Error: Div by 0");
                    return;
                }
                break;
            case "x²":
                firstNumber = Math.pow(firstNumber, 2);
                result = firstNumber;
                break;
            case "mod":
                firstNumber=firstNumber%secondNumber;
                result=firstNumber;
                break;
            case "xª":
                firstNumber=Math.pow(firstNumber,secondNumber);
                result=firstNumber;
                break;

        }
        changeText(result);
        firstNumber = result;
    }

    public void changeText(double n) {
        resultText.setText(String.valueOf(n));
    }
}
