import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import org.json.JSONObject;

class CurrencyConverter extends JFrame {
    private JLabel amount;
    private JTextField amountField;
    private JLabel convertFrom;
    private JTextField convertFromField;
    private JLabel convertTo;
    private JTextField convertToField;
    private JLabel resultLabel;
    private JButton convertButton;
    private JTextField resultField;

    public CurrencyConverter() {
        setTitle("Currency Converter");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        ImageIcon icon = new ImageIcon("/Icon.png");
        setIconImage(icon.getImage());
        setLocationRelativeTo(null);
        setSize(1000, 600);
        setBackground(Color.WHITE);
        setLayout(new GridLayout(8, 1));
        setTextFieldsAndLabels();
        setVisible(true);
    }

    public void setTextFieldsAndLabels() {
        amount = new JLabel("Amount:");
        amountField = new JTextField();
        convertFrom = new JLabel("From:");
        convertFromField = new JTextField();
        convertTo = new JLabel("To:");
        convertToField = new JTextField();
        convertButton = new JButton("Convert");
        resultLabel = new JLabel("Result:");
        resultField = new JTextField();
        resultField.setEditable(false);

        convertButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                convertCurrency();
            }
        });

        add(amount);
        add(amountField);
        add(convertFrom);
        add(convertFromField);
        add(convertTo);
        add(convertToField);
        add(convertButton);
        add(resultLabel);
        add(resultField);
    }

    public void convertCurrency() {
        try {
            String fromCurrency = convertFromField.getText().toUpperCase();
            String toCurrency = convertToField.getText().toUpperCase();
            double amount = Double.parseDouble(amountField.getText());

            String urlStr = "https://api.frankfurter.app/latest?amount=" + amount + "&from=" + fromCurrency + "&to=" + toCurrency;
            URL url = new URL(urlStr);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("GET");

            BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String inputLine;
            StringBuilder content = new StringBuilder();
            while ((inputLine = in.readLine()) != null) {
                content.append(inputLine);
            }
            in.close();
            conn.disconnect();

            JSONObject json = new JSONObject(content.toString());
            double result = json.getJSONObject("rates").getDouble(toCurrency);
            double convertedAmount = rate * amount;
            resultField.setText(String.valueOf(convertedAmount));
        } catch (Exception e) {
            e.printStackTrace();
            resultField.setText("Error");
        }
    }

    public static void main(String[] args) {

        new CurrencyConverter();

    }
}
