import javafx.application.Application;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.stage.Stage;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class WiFiController
{

    /*
    * IP address of ESP8266 module (local network).
    *  */
    private static final String ESP8266_IP = "10.99.145.245";
    /*
    * Port used for TCP connection between ESP8266 and PC app.
    * */
    private static final int ESP8266_PORT = 8084;

    @FXML public Button startButton;
    @FXML public Button applyButton;
    @FXML public Button resetButton;
    @FXML public Button exitButton;

    /*
    * Circles used to represent left and right cup of real system and
    * corresponding color.
    *  */
    @FXML private Circle leftToken;
    @FXML private Circle rightToken;

    /*
    * Toggle groups for radio buttons
    * */
    private final ToggleGroup redToggleGroup = new ToggleGroup();
    private final ToggleGroup greenToggleGroup = new ToggleGroup();
    private final ToggleGroup blueToggleGroup = new ToggleGroup();
    private final ToggleGroup yellowToggleGroup = new ToggleGroup();
    private final ToggleGroup purpleToggleGroup = new ToggleGroup();
    private final ToggleGroup orangeToggleGroup = new ToggleGroup();
    private final ToggleGroup blackToggleGroup = new ToggleGroup();
    private final ToggleGroup whiteToggleGroup = new ToggleGroup();

    /*
    * Radio button fields used to allow user to choose direction of
    * sorted tokens.
    * */
    @FXML private RadioButton redLeft;
    @FXML private RadioButton redRight;
    @FXML private RadioButton greenLeft;
    @FXML private RadioButton greenRight;
    @FXML private RadioButton blueLeft;
    @FXML private RadioButton blueRight;
    @FXML private RadioButton yellowLeft;
    @FXML private RadioButton yellowRight;
    @FXML private RadioButton purpleLeft;
    @FXML private RadioButton purpleRight;
    @FXML private RadioButton orangeLeft;
    @FXML private RadioButton orangeRight;
    @FXML private RadioButton blackLeft;
    @FXML private RadioButton blackRight;
    @FXML private RadioButton whiteLeft;
    @FXML private RadioButton whiteRight;

    /*
    * Probably will be used for synchronized color change in GUI.
    * Just for testing purposes right now.
    * */
    private static String color = "A";

    // Network communication
    private Socket clientSocket;  /* trying to set WI-FI as client here in app */
    private PrintWriter out; /* used for writing to WI-FI */
    private BufferedReader in; /* i'll see about this, perhaps it'll screw the connection or stop it */
    private ExecutorService executorService; /* not sure about this but it works, so I guess ok*/
    private volatile boolean running = true;
    private volatile boolean resetActivated = false;
    private volatile boolean startActivated = false;
    private volatile boolean applyActivated = false;

    public void start(Stage primaryStage) throws Exception
    {
        FXMLLoader loader = new FXMLLoader(getClass().getResource("WiFiController.fxml"));
        Parent root = loader.load();
        Scene scene = new Scene(root, 900, 600);
        WiFiController controller = loader.getController();
        /*
        * Just checking if these tokens are initialized.
        * */
        System.out.println("Controller tokens: " + controller.rightToken + ", " + controller.leftToken);

        primaryStage.setTitle("WiFi Controller");
        primaryStage.setScene(scene);
        primaryStage.show();

        //Platform.runLater(this::startServer); // this worked actually in some way
        // startServer();
        startJustToChangeColorTest();
        /*
        * TODO: add new method call that will handle color change
        * */
    }

    /*
    * TODO: Perhaps change this so it works as if PC app is actually client
    *  and WiFi is server?
    *  UPDATE: Yes, did that.
    * */

    private void startJustToChangeColorTest()
    {
        executorService = Executors.newSingleThreadExecutor();

        executorService.execute(() ->
        {
           String data = "A";
           processIncomingMessage(data);
           sleep();
           data = "B";
            processIncomingMessage(data);
            sleep();
            data = "C";
            processIncomingMessage(data);
            sleep();
            data = "D";
            processIncomingMessage(data);
            sleep();
            data = "E";
            processIncomingMessage(data);
            sleep();
            data = "G";
            processIncomingMessage(data);
            sleep();
            data = "H";
            processIncomingMessage(data);
            sleep();
           data = "h";
           processIncomingMessage(data);
           sleep();
           data = "f";
            processIncomingMessage(data);
            sleep();
            data = "g";
            processIncomingMessage(data);
            sleep();
            data = "d";
            processIncomingMessage(data);
            sleep();
            data = "c";
            processIncomingMessage(data);
            sleep();
            processIncomingMessage(data);
            data = "b";
            sleep();
            processIncomingMessage(data);
            sleep();
            data = "a";
            processIncomingMessage(data);
            sleep();

        });
    }

private void sleep()
{
    long millis = 1000;
    try {
        Thread.sleep(millis);
    } catch (InterruptedException e) {
        throw new RuntimeException(e);
    }
}
    private void startServer()
    {
        executorService = Executors.newSingleThreadExecutor();
        executorService.execute(() ->
        {
            try (ServerSocket serverSocket = new ServerSocket(ESP8266_PORT))
            {
                System.out.println("Server started on port " + ESP8266_PORT);

                while (running) {
                    try {
                        clientSocket = serverSocket.accept();
                        System.out.println("Client connected: " + clientSocket.getInetAddress());

                        // Set up communication streams
                        out = new PrintWriter(clientSocket.getOutputStream(), true);
                        in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

                        // Listen for incoming messages
                        String inputLine;
                        while ((inputLine = in.readLine()) != null && running) {
                            System.out.println("Received: " + inputLine);
                            if(!inputLine.startsWith("AT+")) {
                                // Process each character in the inputLine
                                for (int i = 0; i < inputLine.length(); i++) {
                                    char c = inputLine.charAt(i);
                                    // Send or process each character
                                    processIncomingMessage(String.valueOf(c));  // if your method expects a String
                                    // or if your method can handle chars directly:
                                    // processIncomingMessage(c);
                                    // sendData("HELLO\r\n");
                                }
                            }
                        }

                        clearInputBuffer();

                    } catch (IOException e) {
                        System.err.println("Connection error: " + e.getMessage());
                    } finally {
                        cleanupConnection();
                    }

                    // Short delay before accepting new connection
                    //try { Thread.sleep(1000); } catch (InterruptedException e) { break; }
                }
            } catch (IOException e) {
                System.err.println("Server error: " + e.getMessage());
            }
        });
    }

    private void stopServer() {
        running = false;
        cleanupConnection();
        if (executorService != null) {
            executorService.shutdownNow();
        }
    }
    private void clearInputBuffer() throws IOException {
        char[] buffer = new char[1024];
        while (in.ready()) {
            int bytesRead = in.read(buffer, 0, buffer.length);
            if (bytesRead == -1) break;
        }
    }
    private void cleanupConnection() {
        try {
            if (out != null) out.close();
            if (in != null) in.close();
            if (clientSocket != null && !clientSocket.isClosed()) {
                clientSocket.close();
            }
        } catch (IOException e) {
            System.err.println("Error closing connection: " + e.getMessage());
        }
    }

    private void processIncomingMessage(String message) {
        // Filter out AT commands and empty messages
        if (message.isEmpty() || message.startsWith("AT+")) {
            return;
        }

        // Update GUI based on received color code
        Platform.runLater(() -> {
            changeCircleColorBasedOnData(message.trim());
        });
    }

    // Send data to microcontroller
    public void sendData(String data) {
        if (out != null && !clientSocket.isClosed()) {
            out.println(data);
            System.out.println("Sent to ESP8266: " + data);
        } else {
            System.err.println("Cannot send data - no active connection");
            Platform.runLater(this::showConnectionError);
        }
    }

    private void showConnectionError() {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("Connection Error");
        alert.setHeaderText("Not connected to microcontroller");
        alert.setContentText("Please ensure the ESP8266 is powered on and connected to the same network.");
        alert.showAndWait();
    }

    // Existing color change methods with slight modifications
    @FXML
    public void changeCircleColorBasedOnData(String command) {
        if(resetActivated)
        {
            return;
        }
        System.out.println("Processing command: " + command);

        switch (command) {
            case "A": changeRightTokenColor(Color.RED); break;
            case "B": changeRightTokenColor(Color.BLUE); break;
            case "C": changeRightTokenColor(Color.GREEN); break;
            case "D": changeRightTokenColor(Color.YELLOW); break;
            case "E": changeRightTokenColor(Color.PURPLE); break;
            case "F": changeRightTokenColor(Color.ORANGE); break;
            case "G": changeRightTokenColor(Color.BLACK); break;
            case "H": changeRightTokenColor(Color.WHITE); break;
            case "a": changeLeftTokenColor(Color.RED); break;
            case "b": changeLeftTokenColor(Color.BLUE); break;
            case "c": changeLeftTokenColor(Color.GREEN); break;
            case "d": changeLeftTokenColor(Color.YELLOW); break;
            case "e": changeLeftTokenColor(Color.PURPLE); break;
            case "f": changeLeftTokenColor(Color.ORANGE); break;
            case "g": changeLeftTokenColor(Color.BLACK); break;
            case "h": changeLeftTokenColor(Color.WHITE); break;
            default:
                System.err.println("Unknown command: " + command);
        }
    }

    // Button handlers
    @FXML
    public void handleButtonAction(ActionEvent actionEvent) {
        // sendData("START");
        startActivated = true;
        resetActivated = false;
        applyActivated = false;
        sendData("S\r\n");
        /*if(!applyActivated) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Not Finished Setup");
            alert.setHeaderText("You must choose direction for each color!");
            alert.setContentText("");
            alert.showAndWait();
            return;
        }*/
    }
    private static String bytesToHex(byte[] bytes, int length)
    {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < length; i++)
        {
            sb.append(String.format("%02X ", bytes[i]));
        }
        return sb.toString();
    }
    public static String bytesToAscii(byte[] hexBytes)
    {
        StringBuilder sb = new StringBuilder();
        for (byte b : hexBytes)
        {
            sb.append((char) b);
        }
        return sb.toString();
    }
    public String getSelectedColorSides()
    {
        if (redToggleGroup.getSelectedToggle() == redLeft) return "Left Red";
        if (redToggleGroup.getSelectedToggle() == redRight) return "Right Red";

        if (greenToggleGroup.getSelectedToggle() == greenLeft) return "Left Green";
        if (greenToggleGroup.getSelectedToggle() == greenRight) return "Right Green";

        if (blueToggleGroup.getSelectedToggle() == blueLeft) return "Left Blue";
        if (blueToggleGroup.getSelectedToggle() == blueRight) return "Right Blue";

        if (yellowToggleGroup.getSelectedToggle() == yellowLeft) return "Left Yellow";
        if (yellowToggleGroup.getSelectedToggle() == yellowRight) return "Right Yellow";

        if (purpleToggleGroup.getSelectedToggle() == purpleLeft) return "Left Purple";
        if (purpleToggleGroup.getSelectedToggle() == purpleRight) return "Right Purple";

        if (orangeToggleGroup.getSelectedToggle() == orangeLeft) return "Left Orange";
        if (orangeToggleGroup.getSelectedToggle() == orangeRight) return "Right Orange";

        if (blackToggleGroup.getSelectedToggle() == blackLeft) return "Left Black";
        if (blackToggleGroup.getSelectedToggle() == blackRight) return "Right Black";

        if (whiteToggleGroup.getSelectedToggle() == whiteLeft) return "Left White";
        if (whiteToggleGroup.getSelectedToggle() == whiteRight) return "Right White";

        return "No selection";
    }

    @FXML
    public void changeLeftTokenColor(Color color)
    {
        Objects.requireNonNull(leftToken, "leftToken not initialized!");
        leftToken.setFill(color);

    }
    @FXML
    public void changeRightTokenColor(Color color)
    {
        Objects.requireNonNull(rightToken, "rightToken not initialized!");
        rightToken.setFill(color);
    }

    @FXML
    public void handleApplyCommand(ActionEvent actionEvent)
    {
        /*
        * Some nonsense function body.
        * */
        //changeCircleColorBasedOnData(color);
        applyActivated = true;
        /*
        * TODO: implement save option.
        * */
    }

    @FXML
    public void handleResetAction(ActionEvent actionEvent)
    {
        resetActivated = true;
        startActivated = false;
        sendData("R\r\n");

        if (executorService != null)
        {
            executorService.shutdownNow();
            executorService = null;
        }

        // Reset all radio buttons
        redToggleGroup.selectToggle(null);
        greenToggleGroup.selectToggle(null);
        blueToggleGroup.selectToggle(null);
        yellowToggleGroup.selectToggle(null);
        purpleToggleGroup.selectToggle(null);
        orangeToggleGroup.selectToggle(null);
        blackToggleGroup.selectToggle(null);
        whiteToggleGroup.selectToggle(null);

        // Reset token colors
        changeLeftTokenColor(Color.BLUE);
        changeRightTokenColor(Color.RED);
        startJustToChangeColorTest();

    }

    @FXML
    public void initialize()
    {
        System.out.println("Initializing controller...");
        System.out.println("rightToken: " + (rightToken != null ? "set" : "NULL"));
        System.out.println("leftToken: " + (leftToken != null ? "set" : "NULL"));
        // Initialize toggle set
        redLeft.setToggleGroup(redToggleGroup);
        redRight.setToggleGroup(redToggleGroup);

        greenLeft.setToggleGroup(greenToggleGroup);
        greenRight.setToggleGroup(greenToggleGroup);

        blueLeft.setToggleGroup(blueToggleGroup);
        blueRight.setToggleGroup(blueToggleGroup);

        yellowLeft.setToggleGroup(yellowToggleGroup);
        yellowRight.setToggleGroup(yellowToggleGroup);

        purpleLeft.setToggleGroup(purpleToggleGroup);
        purpleRight.setToggleGroup(purpleToggleGroup);

        orangeLeft.setToggleGroup(orangeToggleGroup);
        orangeRight.setToggleGroup(orangeToggleGroup);

        blackLeft.setToggleGroup(blackToggleGroup);
        blackRight.setToggleGroup(blackToggleGroup);

        whiteLeft.setToggleGroup(whiteToggleGroup);
        whiteRight.setToggleGroup(whiteToggleGroup);

        changeLeftTokenColor(Color.BLUE);
        changeRightTokenColor(Color.RED);

       new Thread(() -> {
            try {
                // Small delay to ensure UI is fully loaded
                Thread.sleep(500);
                // or startServer();
                Platform.runLater(this::startServer);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }).start();

    }

    @FXML
    public void handleExitAction(ActionEvent event) {
        Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
        alert.setTitle("Exit App");
        alert.setHeaderText("Are you sure you want to exit app?");
        alert.setContentText("All active processes will be shutdown.");

        Optional<ButtonType> result = alert.showAndWait();
        if (result.isPresent() && result.get() == ButtonType.OK) {
            stopAllProcesses();
            Platform.exit();
            System.exit(0);
        }
    }

    private void stopAllProcesses() {

        startActivated = false;
        resetActivated = true;

        running = false;

        if (executorService != null) {
            executorService.shutdownNow();
        }

        if (executorService != null) {
            executorService.shutdownNow();
        }

        cleanupConnection();

        System.out.println("Goodbye. Resources freeing...");
    }

}