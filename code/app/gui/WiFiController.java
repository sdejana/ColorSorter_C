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

/**
 * The WiFiController class is a JavaFX controller that manages the GUI and network communication
 * for a system that interacts with an PIC24FJ64GA002 microcontroller. It allows users to control
 * and monitor color tokens (represented as circles in the UI) through a WiFi (ESP8266) connection.
 *
 * <p>The controller provides functionality to:
 * <ul>
 *   <li>Start and reset the color sorting process</li>
 *   <li>Configure color directions (left or right) for each token color</li>
 *   <li>Display real-time color changes received from the microcontroller</li>
 *   <li>Manage network connections with the ESP8266 module</li>
 *   <li>Handle application exit and resource cleanup</li>
 * </ul>
 *
 * <p>Communication with the ESP8266 is done via TCP sockets, with the PC application acting as
 * a server that listens for connections from the microcontroller.
 */

public class WiFiController
{

    /**
     * IP address of the ESP8266 module on the local network.
     */
    private static final String ESP8266_IP = "10.99.145.245";

    /**
     * Port number used for TCP communication between the application and ESP8266.
     */
    private static final int ESP8266_PORT = 8084;

    /**
     * Button to start the color sorting process.
     */
    @FXML public Button startButton;

    /**
     * Button to apply color direction configurations.
     */
    @FXML public Button applyButton;

    /**
     * Button to reset the system to its initial state.
     */
    @FXML public Button resetButton;

    /**
     * Button to exit the application.
     */
    @FXML public Button exitButton;

    /**
     * Circle representing the left token in the UI.
     */
    @FXML private Circle leftToken;

    /**
     * Circle representing the right token in the UI.
     */
    @FXML private Circle rightToken;

    // Toggle groups for color direction radio buttons
    private final ToggleGroup redToggleGroup = new ToggleGroup();
    private final ToggleGroup greenToggleGroup = new ToggleGroup();
    private final ToggleGroup blueToggleGroup = new ToggleGroup();
    private final ToggleGroup yellowToggleGroup = new ToggleGroup();
    private final ToggleGroup purpleToggleGroup = new ToggleGroup();
    private final ToggleGroup orangeToggleGroup = new ToggleGroup();
    private final ToggleGroup blackToggleGroup = new ToggleGroup();
    private final ToggleGroup whiteToggleGroup = new ToggleGroup();

    // Radio buttons for color directions
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

    // Network communication fields
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;
    private ExecutorService executorService;

    // Flags
    private volatile boolean appRunning = false;
    private volatile boolean resetActivated = false;
    private volatile boolean startActivated = false;
    private volatile boolean applyActivated = false;
    private volatile boolean serverRunning = true;

    /**
     * Initializes and starts the JavaFX application.
     *
     * @param primaryStage The primary stage for this application.
     * @throws Exception if there is an error loading the FXML file.
     */
    public void start(Stage primaryStage) throws Exception {
        FXMLLoader loader = new FXMLLoader(getClass().getResource("WiFiController.fxml"));
        Parent root = loader.load();
        Scene scene = new Scene(root, 900, 600);
        WiFiController controller = loader.getController();

        System.out.println("Controller tokens: " + controller.rightToken + ", " + controller.leftToken);

        primaryStage.setTitle("WiFi Controller");
        primaryStage.setScene(scene);
        primaryStage.show();

    }

    /**
     * Starts the server that listens for connections from the ESP8266.
     * The server runs in a separate thread to avoid blocking the UI.
     */
    private void startServer() {
        executorService = Executors.newSingleThreadExecutor();
        executorService.execute(() -> {
            try (ServerSocket serverSocket = new ServerSocket(ESP8266_PORT)) {
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
                }
            } catch (IOException e) {
                System.err.println("Server error: " + e.getMessage());
            }
        });
    }

    /**
     * Stops the server and cleans up resources.
     */
    private void stopServer() {
        serverRunning = false;
        cleanupConnection();
        if (executorService != null) {
            executorService.shutdownNow();
        }
    }

    /**
     * Clears the input buffer to remove any remaining data.
     *
     * @throws IOException if an I/O error occurs while clearing the buffer.
     */
    private void clearInputBuffer() throws IOException {
        char[] buffer = new char[1024];
        while (in.ready()) {
            int bytesRead = in.read(buffer, 0, buffer.length);
            if (bytesRead == -1) break;
        }
    }

    /**
     * Cleans up network connection resources.
     */
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

    /**
     * Processes incoming messages from the ESP8266.
     *
     * @param message The message received from the microcontroller.
     */
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
        }
        else {
            // Update GUI based on received color code if none of above is true
            Platform.runLater(() -> {
                changeCircleColorBasedOnData(message.trim());
            });
        }
    }

    /**
     * Sends data to the connected ESP8266 microcontroller.
     *
     * @param data The data to be sent to the microcontroller.
     */
    public void sendData(String data) {
        if (out != null && !clientSocket.isClosed()) {
            out.println(data);
            System.out.println("Sent to ESP8266: " + data);
        } else {
            System.err.println("Cannot send data - no active connection");
            Platform.runLater(this::showConnectionError);
        }
    }

    /**
     * Shows a connection error dialog when communication with the ESP8266 fails.
     */
    private void showConnectionError() {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("Connection Error");
        alert.setHeaderText("Not connected to microcontroller");
        alert.setContentText("Please ensure the ESP8266 is powered on and connected to the same network.");
        alert.showAndWait();
    }

    /**
     * Changes the color of the circles based on commands received from the ESP8266.
     *
     * @param command The command specifying which color to display and on which side.
     */
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


    /**
     * Handles the start button action, initiating the color sorting process.
     *
     * @param actionEvent The event triggered by clicking the start button.
     */
    @FXML
    public void handleButtonAction(ActionEvent actionEvent) {
    
        startActivated = true;
        resetActivated = false;
        applyActivated = false;

        sendData("<S>S<E>\r\n");

        appRunning = true;
    }

    /**
     * Gets the currently selected color directions as a formatted string.
     *
     * @return A string containing all selected color directions in the format "a#A#c#C#..." etc.
     */
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

    /**
     * Changes the color of the left token circle.
     *
     * @param color The new color for the left token.
     */
    @FXML
    public void changeLeftTokenColor(Color color) {
        Objects.requireNonNull(leftToken, "leftToken not initialized!");
        leftToken.setFill(color);

    }

    /**
     * Changes the color of the right token circle.
     *
     * @param color The new color for the right token.
     */
    @FXML
    public void changeRightTokenColor(Color color) {
        Objects.requireNonNull(rightToken, "rightToken not initialized!");
        rightToken.setFill(color);
    }


    /**
     * Handles the apply button action, sending color direction configurations to the ESP8266.
     *
     * @param actionEvent The event triggered by clicking the apply button.
     */
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
            // Sending data in format: <S> data <E>
            sendData("<S>"+selColors+"<E>\r\n");
        }
        /*
        * TODO: implement save option.
        * */
    }

    /**
     * Handles the reset button action, resetting the system to its initial state.
     *
     * @param actionEvent The event triggered by clicking the reset button.
     */
    @FXML
    public void handleResetAction(ActionEvent actionEvent)
    {
        resetActivated = true;
        startActivated = false;
        appRunning = false;

        sendData("<S>R<E>\r\n");

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

    /**
     * Initializes the controller after its root element has been processed.
     * Sets up toggle groups, initial colors, and starts the server.
     */
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

    /**
     * Handles the exit button action, showing a confirmation dialog before exiting.
     *
     * @param event The event triggered by clicking the exit button.
     */
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

    /**
     * Stops all running processes and cleans up resources before application exit.
     */
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