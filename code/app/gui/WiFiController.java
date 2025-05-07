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
    @FXML public Button resetConnectionButton;


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

    // Network communication
    private Socket clientSocket;  /* trying to set WI-FI as client here in app */
    private PrintWriter out; /* used for writing to WI-FI */
    private BufferedReader in; /* I'll see about this, perhaps it'll screw the connection or stop it */
    private ExecutorService executorService; /* not sure about this but it works, so I guess ok*/
    private volatile boolean appRunning = false;
    private volatile boolean resetActivated = false;
    private volatile boolean startActivated = false;
    private volatile boolean applyActivated = false;
    private volatile boolean serverRunning = true;

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
        // startJustToChangeColorTest();
        /*
         * TODO: add new method call that will handle color change: NO NEED
         * */
    }

    /*
     * TODO: Perhaps change this so it works as if PC app is actually client
     *  and WiFi is server?
     *  UPDATE: Yes, did that.
     * */

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

                while (serverRunning) {
                    try {
                        clientSocket = serverSocket.accept();
                        System.out.println("Client connected: " + clientSocket.getInetAddress());

                        // Set up communication streams
                        out = new PrintWriter(clientSocket.getOutputStream(), true);
                        in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

                        // Listen for incoming messages
                        String inputLine;
                        while ((inputLine = in.readLine()) != null && serverRunning) {
                            System.out.println("Received: " + inputLine);
                            inputLine = inputLine.replace("AT+","");

                            if(!inputLine.startsWith("AT+")) {
                                // Process each character in the inputLine
                                for (int i = 0; i < inputLine.length(); i++) {
                                    char c = inputLine.charAt(i);
                                    // Send or process each character
                                    processIncomingMessage(String.valueOf(c));
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
        serverRunning = false;
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
        if(message.trim().equals("S")) // Microcontroller sent START, so button has been pressed
        {
            serverRunning = true;
            appRunning = true;
            startActivated = true;
            resetActivated = false;
        } else if (message.trim().equals("R")) { // Microcontroller sent RESET, so button has been pressed again
            serverRunning = true;
            appRunning = false;
            startActivated = false;
            resetActivated = true;
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

        }
        else {
            // Update GUI based on received color code if none of above is true
            Platform.runLater(() -> {
                changeCircleColorBasedOnData(message.trim());
            });
        }
    }

    // Send data to microcontroller
    public void sendData(String data) {
        if (out != null && !clientSocket.isClosed()) {
            out.println(data);
            out.flush(); // added this for force sending
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
            case "E": changeRightTokenColor(Color.DEEPPINK); break;
            case "F": changeRightTokenColor(Color.ORANGE); break;
            case "G": changeRightTokenColor(Color.BLACK); break;
            case "H": changeRightTokenColor(Color.WHITE); break;
            case "a": changeLeftTokenColor(Color.RED); break;
            case "b": changeLeftTokenColor(Color.BLUE); break;
            case "c": changeLeftTokenColor(Color.GREEN); break;
            case "d": changeLeftTokenColor(Color.YELLOW); break;
            case "e": changeLeftTokenColor(Color.DEEPPINK); break;
            case "f": changeLeftTokenColor(Color.ORANGE); break;
            case "g": changeLeftTokenColor(Color.BLACK); break;
            case "h": changeLeftTokenColor(Color.WHITE); break;
            default:
                System.err.println("Unknown command: " + command);
        }
    }

    // Button handlers
    /* start button */
    @FXML
    public void handleButtonAction(ActionEvent actionEvent) {

        startActivated = true;
        resetActivated = false;
        applyActivated = false;

        sendData("SSS\r\n");

        appRunning = true;
    }

    public String getSelectedColorSides()
    {
        String selection = "";

        if (redToggleGroup.getSelectedToggle() == redLeft) selection += "a#";
        if (redToggleGroup.getSelectedToggle() == redRight) selection += "A#";

        if (greenToggleGroup.getSelectedToggle() == greenLeft) selection += "c#";
        if (greenToggleGroup.getSelectedToggle() == greenRight) selection += "C#";

        if (blueToggleGroup.getSelectedToggle() == blueLeft) selection += "b#";
        if (blueToggleGroup.getSelectedToggle() == blueRight) selection += "B#";

        if (yellowToggleGroup.getSelectedToggle() == yellowLeft) selection += "d#";
        if (yellowToggleGroup.getSelectedToggle() == yellowRight) selection += "D#";

        if (purpleToggleGroup.getSelectedToggle() == purpleLeft) selection += "e#";
        if (purpleToggleGroup.getSelectedToggle() == purpleRight) selection += "E#";

        if (orangeToggleGroup.getSelectedToggle() == orangeLeft) selection += "f#";
        if (orangeToggleGroup.getSelectedToggle() == orangeRight) selection += "F#";

        if (blackToggleGroup.getSelectedToggle() == blackLeft) selection += "g#";
        if (blackToggleGroup.getSelectedToggle() == blackRight) selection += "G#";

        if (whiteToggleGroup.getSelectedToggle() == whiteLeft) selection += "h#";
        if (whiteToggleGroup.getSelectedToggle() == whiteRight) selection += "H#";

        return selection;
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
        applyActivated = true;
        String selColors = getSelectedColorSides();
        if(selColors.length() != 16)
        {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Not Finished Setup");
            alert.setHeaderText("You must choose direction for each color!");
            alert.setContentText("Every color must have direction - left or right.");
            alert.showAndWait();
        }
        else
        {
            /* Sending data in format: <S> data <E>*/
            sendData("0"+selColors+"1\r\n");
        }
        /*
         * TODO: implement save option.
         * */
    }

    @FXML
    public void handleResetConnectionAction(ActionEvent actionEvent) {
        System.out.println("Resetting connection...");

        // Stop the current connection
        stopServer();

        // Clear any existing connection state
        cleanupConnection();

        // Reset flags
        serverRunning = true;
        appRunning = false;

        // Restart the server
        startServer();

        // Show confirmation
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("Connection Reset");
            alert.setHeaderText("Connection has been reset");
            alert.setContentText("Attempting to reconnect to the ESP8266 module...");
            alert.showAndWait();
        });
    }

    @FXML
    public void handleResetAction(ActionEvent actionEvent)
    {
        resetActivated = true;
        startActivated = false;
        appRunning = false;

        sendData("EEE\r\n");

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
                //Platform.runLater(this::startJustToChangeColorTest);
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

        appRunning = serverRunning = false;

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