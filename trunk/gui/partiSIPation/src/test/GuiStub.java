package test;

import javax.swing.ImageIcon;
import javax.swing.JOptionPane;

/**
 * This class provides all functions of the GUI to the core.
 * 
 * @author Oleksiy Reznikov
 * @author Anton Huttenlocher
 *  
 */
public class GuiStub {

    //private Client gui;
    private Gui gui;
    private ClassLoader cl;
    /* public GuiStub(Client g) {
        gui = g;
    } */
    
    public GuiStub(Gui g) {
        gui = g;
        cl = gui.cl;
    }

    public boolean changeRegStatus(int accountId, boolean registered) {
        gui.jTextArea1.append("CHANGE REG STATUS" + "\n");
        return true;
    }

    public boolean changeCallStatus(int callId, String callStatus) {
        gui.jTextArea1.append("Call status is: "+callStatus+"\n");
        if (callStatus.equalsIgnoreCase("ACCEPTED")) {
            gui.phone_down_active = true;
            gui.jLabel14.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_down.gif")));
        }
        //gui.jTextArea1.append("CHANGE CALL STATUS" + "\n");
        return true;
    }

    public boolean showUserEvent(int accountId, String category, String title,
            String message, String details) {
        gui.jTextArea1.append("SHOW USER EVENT" + "\n");
        return true;
    }

    public boolean registerCore() {
        gui.jTextArea1.append("REGISTER CORE" + "\n");
        return true;
    }

    public boolean incomingCall(int accountId, int callId, String sipUri,
            String displayName) {
        Object[] options = {"Accept", "Decline"};
        int n = JOptionPane.showOptionDialog(gui,
                "Incoming call from "
                + sipUri + " ("+ displayName +").\n",
                "INCOMING CALL", JOptionPane.YES_NO_OPTION,
                JOptionPane.WARNING_MESSAGE, null, options, options[0]);

        gui.jTextArea1.append(Utils.getTimestamp()+": Call from "+ sipUri + " ("+ displayName +") "); 
        if (n == 0) {
            gui.jTextArea1.append("accepted.\n");
            gui.phone_down_active = true;
            gui.jLabel14.setEnabled(true);
            gui.list2.addElement(sipUri + " ("+ displayName +")");
            gui.jLabel14.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_down.gif")));
        } else {
            gui.jTextArea1.append("declined.\n");
        }
        return true;
    }

    public boolean setSpeakerVolume(double level) {
        gui.jSlider1.setValue((int)(level * 100));
        gui.jTextArea1.append("SET SPEAKER VOLUME" + "\n");
        return true;
    }

    public boolean setMicroVolume(double level) {
        gui.jTextArea1.append("SET MICRO VOLUME" + "\n");
        return true;
    }

}
