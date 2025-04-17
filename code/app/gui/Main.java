import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

/**
 * The Main class serves as the entry point for the JavaFX Wi-Fi Controller application.
 * It initializes and launches the primary application window that contains the
 * Wi-Fi controller interface.
 *
 * <p>This class extends {@link javafx.application.Application} and implements the
 * standard JavaFX application lifecycle. The main UI is loaded from an FXML file
 * and displayed in a primary stage (window).
 *
 * @see javafx.application.Application
 */
public class Main extends Application {

    /**
     * The main entry point for the JavaFX application.
     *
     * <p>This method:
     * <ul>
     *   <li>Loads the FXML file defining the user interface</li>
     *   <li>Creates a scene containing the loaded UI hierarchy</li>
     *   <li>Sets up the primary stage (main window) with the scene</li>
     *   <li>Displays the primary stage</li>
     * </ul>
     *
     * @param primaryStage The primary stage for this application, onto which
     *                     the application scene can be set.
     * @throws Exception if there is an error loading the FXML file or
     *                   creating the scene.
     */
    @Override
    public void start(Stage primaryStage) throws Exception {
        FXMLLoader loader = new FXMLLoader(getClass().getResource("WiFiController.fxml"));
        Parent root = loader.load();
        Scene scene = new Scene(root, 900, 600);
        primaryStage.setTitle("WiFi Controller");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    /**
     * The entry point of the application.
     *
     * <p>This method launches the JavaFX application by calling
     * {@link Application#launch(String...)} with the provided command line arguments.
     *
     * @param args The command line arguments passed to the application.
     */
    public static void main(String[] args) {
        launch(args);
    }
}