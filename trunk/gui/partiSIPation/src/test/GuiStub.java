package test;

/**
 * This class provides all functions of the GUI to the core.
 * 
 * @author Oleksiy Reznikov
 * @author Anton Huttenlocher
 *  
 */
public class GuiStub {

    private Client gui;

    public GuiStub(Client g) {
        gui = g;
    }

    public boolean changeRegStatus(int accountId, boolean registered) {
        gui.textarea_LOG.append("CHANGE REG STATUS" + "\n");
        return true;
    }

    public boolean changeCallStatus(int callId, String callStatus) {
        gui.textarea_LOG.append("CHANGE CALL STATUS" + "\n");
        return true;
    }

    public boolean showUserEvent(int accountId, String category, String title,
            String message, String details) {
        gui.textarea_LOG.append("SHOW USER EVENT" + "\n");
        return true;
    }

    public boolean registerCore() {
        gui.textarea_LOG.append("REGISTER CORE" + "\n");
        return true;
    }

    public boolean incomingCall(int accountId, int callId, String sipUri,
            String displayName) {
        gui.textarea_LOG.append("INCOMING CALL" + "\n");
        return true;
    }

    public boolean setSpeakerVolume(double level) {
        gui.textarea_LOG.append("SET SPEAKER VOLUME" + "\n");
        return true;
    }

    public boolean setMicroVolume(double level) {
        gui.textarea_LOG.append("SET MICRO VOLUME" + "\n");
        return true;
    }

}
