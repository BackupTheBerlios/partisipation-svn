package gui;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeEvent;
import java.util.Enumeration;
import java.util.Vector;
import javax.swing.ImageIcon;
import javax.swing.JDialog;
import javax.swing.JOptionPane;

/**
 * This class provides all functions of the GUI to the core.
 * 
 * @author Oleksiy Reznikov
 * @author Anton Huttenlocher
 *  
 */
public class GuiStub {

    private Gui gui;

    private ClassLoader cl;

    private Vector pendingCalls = new Vector();

    private Vector params = new Vector();
    
    private JDialog dialog;

    public GuiStub(Gui g) {
        gui = g;
        cl = gui.cl;
        dialog = new JDialog(gui, "Incoming call", true);
    }

    /**
     * This method changes registration status of an account.
     * 
     * @param accountId
     * @param registered
     * @return true = success, false = error
     */
    public boolean changeRegStatus(int accountId, boolean registered) {

        try {
            Enumeration e = gui.accounts.elements();
            int i = 0;
            while (e.hasMoreElements()) {
                Account acc = (Account) e.nextElement();
                int n = acc.id;
                if (n == accountId) {
                    if (registered) {
                        acc.setRegistered(true);
                        ImageIcon img = new ImageIcon(cl
                                .getResource("gui/resources/green.gif"));
                        params.clear();
                        params.add(new Integer(n));
                        params.add("name");
                        img.setDescription((String) gui.execute(
                                "core.accountGet", params));
                        gui.list1.set(i, img);

                    } else {
                        acc.setRegistered(false);
                        ImageIcon img = new ImageIcon(cl
                                .getResource("gui/resources/red.gif"));
                        params.clear();
                        params.add(new Integer(n));
                        params.add("name");
                        img.setDescription((String) gui.execute(
                                "core.accountGet", params));
                        gui.list1.set(i, img);
                    }
                    break;
                }
                i++;
            }
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * This method changes the status of a call.
     * 
     * @param callId
     * @param callStatus
     * @return true = success, false = error
     */
    public boolean changeCallStatus(int callId, String callStatus) {
        try {
            
            if (callStatus.equalsIgnoreCase("TERMINATED")) {

                dialog.setVisible(false);
                
                removeCall(callId);
                print("Call # " + callId + " terminated.");

            } else {

                Call c = getCall(callId, gui.calls);

                if (callStatus.equalsIgnoreCase("ACCEPTED")) {
                    updateCall(c, "green");
                    print("Call # " + callId + " accepted.");
                } else if (callStatus.equalsIgnoreCase("TRYING")) {
                    updateCall(c, "gray");
                } else if (callStatus.equalsIgnoreCase("RINGING")) {
                    updateCall(c, "yellow");
                } else if (callStatus.equalsIgnoreCase("DECLINED")) {
                    updateCall(c, "red");
                    print("Call # " + callId + " declined.");
                } else if (callStatus.equalsIgnoreCase("CANCELLED")) {
                    updateCall(c, "red");
                    print("Call # " + callId + " cancelled.");
                } else if (callStatus.equalsIgnoreCase("UNREACHABLE")) {
                    updateCall(c, "gray");
                    print("Call # " + callId + " unreachable.");
                }
            }
            return true;
        } catch (Exception e) {
            {
                e.printStackTrace();
                return false;
            }
        }
    }

    /**
     * This method shows a message box if a user event occures.
     * 
     * @param accountId
     * @param category
     * @param title
     * @param message
     * @param details
     * @return true = success, false = error
     */
    public boolean showUserEvent(int accountId, String category, String title,
            String message, String details) {
        try {

            MessageDialogThread t = new MessageDialogThread(gui, accountId, category, title,
                    message, details);
            t.start();

            return true;
        } catch (Exception e) {
            {
                e.printStackTrace();
                return false;
            }
        }
    }

    /**
     * This method is called for core registration.
     * 
     * @return true = success, false = error
     */
    public boolean registerCore() {
        try {
            print("Core registered.");
            return true;
        } catch (Exception e) {
            {
                e.printStackTrace();
                return false;
            }
        }
    }

    /**
     * This method shows a dialog box to allow the user to accept or to decline an incoming call.
     * 
     * @param accountId
     * @param callId
     * @param sipUri
     * @param displayName
     * @return true = success, false = error
     */
    public boolean incomingCall(int accountId, int callId, String sipUri,
            String displayName) {
        try {
            
            print("Incoming call # " + callId + " from " + sipUri + " ("
                    + displayName + ").");
            
            DialogThread dt = new DialogThread(gui, dialog, pendingCalls, accountId, callId, sipUri, displayName);
            dt.start();
                      
            return true;
        } catch (Exception e) {
            {
                e.printStackTrace();
                return false;
            }
        }
    }
    
    /**
     * This method changes level of speaker volume.
     * 
     * @param level
     * @return true = success, false = error
     */
    public boolean setSpeakerVolume(double level) {
        try {
            gui.jSlider1.setValue((int) (level * 100));
            return true;
        } catch (Exception e) {
            {
                e.printStackTrace();
                return false;
            }
        }
    }

    /**
     * This method changes level of microphone volume.
     * 
     * @param level
     * @return true = success, false = error
     */
    public boolean setMicroVolume(double level) {
        try {
            gui.jSlider2.setValue((int) (level * 100));
            return true;
        } catch (Exception e) {
            {
                e.printStackTrace();
                return false;
            }
        }
    }

    private Call getCall(int n, Vector vec) {
        Enumeration e = vec.elements();
        while (e.hasMoreElements()) {
            Call c = (Call) e.nextElement();
            if (c.id == n) {
                return c;
            }
        }
        return null;
    }

    private void removeCall(int id) {

        int ind;

        if (isInside(id, gui.calls)) {
            ind = getIndex(id, gui.calls);
            gui.calls.remove(ind);
            gui.list2.remove(ind);
        } else if (isInside(id, pendingCalls)) {
            ind = getIndex(id, pendingCalls);
            pendingCalls.remove(ind);
        }
    }

    private boolean isInside(int id, Vector v) {

        Enumeration e = v.elements();

        while (e.hasMoreElements()) {
            Call c = (Call) e.nextElement();
            if (c.id == id) {
                return true;
            }
        }

        return false;
    }

    private int getIndex(int callId, Vector vec) {
        Enumeration e = vec.elements();
        int i = 0;
        while (e.hasMoreElements()) {
            int n = ((Call) e.nextElement()).id;
            if (n == callId) {
                return i;
            }
            i++;
        }
        return -1;
    }

    /**
     * This method updates list line with call information.
     * 
     * @param Call c call to be updated
     * @param String color color of status indicator; possible values are gray, red, green & yellow
     */
    private void updateCall(Call c, String color) {
        try {
            if (c != null) {
            String path = "gui/resources/" + color.toLowerCase() + ".gif";

            ImageIcon img = new ImageIcon(cl.getResource(path));

            if (c.name.trim().equalsIgnoreCase("")) {
                img.setDescription(c.sipurl);
            } else {
                img.setDescription(c.sipurl + " (" + c.name + ")");
            }

            int index = getIndex(c.id, gui.calls);

            gui.list2.set(index, img);
            
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void print(String s) {
        gui.jTextArea1.append(Utils.getTimestamp() + ": " + s + "\n");
    }
}

class MessageDialogThread extends Thread {

    Gui gui;

    int accountId;

    String category;

    String title;

    String message;

    String details;

    public MessageDialogThread(Gui g, int id, String c, String t, String m, String d) {
        gui = g;
        accountId = id;
        category = c;
        title = t;
        message = m;
        details = d;
    }

    public void run() {
        if (category.equalsIgnoreCase("DEBUG")) {
            JOptionPane.showMessageDialog(gui, message + "\n\n" + details,
                    title, JOptionPane.QUESTION_MESSAGE);
        } else if (category.equalsIgnoreCase("INFO")) {
            JOptionPane.showMessageDialog(gui, message + "\n\n" + details,
                    title, JOptionPane.INFORMATION_MESSAGE);
        } else if (category.equalsIgnoreCase("MESSAGE")) {
            JOptionPane.showMessageDialog(gui, message + "\n\n" + details,
                    title, JOptionPane.PLAIN_MESSAGE);
        } else if (category.equalsIgnoreCase("WARNING")) {
            JOptionPane.showMessageDialog(gui, message + "\n\n" + details,
                    title, JOptionPane.WARNING_MESSAGE);
        } else {
            JOptionPane.showMessageDialog(gui, message + "\n\n" + details,
                    title, JOptionPane.ERROR_MESSAGE);
        }
    }
}

class DialogThread extends Thread {
    
    Gui gui;
    JDialog dialog;
    Vector pendingCalls;
    int accountId;
    int callId;
    String sipUri;
    String displayName;
        
    public DialogThread(Gui g, JDialog d, Vector p, int aid, int cid, String uri, String dn) {
        gui = g;
        dialog = d;
        pendingCalls = p;
        accountId = aid;
        callId = cid;
        sipUri = uri;
        displayName = dn;
    }

    public void run() {
        Object[] options = { "Accept", "Decline" };
        
       final JOptionPane optionPane = new JOptionPane(
                "Incoming call from "
                + sipUri + " (" + displayName + ").\n",
                JOptionPane.WARNING_MESSAGE,
                JOptionPane.YES_NO_OPTION,
                null, options, options[0]);
        
       dialog.setContentPane(optionPane);
       
       optionPane.addPropertyChangeListener(
               new PropertyChangeListener() {
                   public void propertyChange(PropertyChangeEvent e) {
                       String prop = e.getPropertyName();

                       if (dialog.isVisible() 
                        && (e.getSource() == optionPane)
                        && (prop.equals(JOptionPane.VALUE_PROPERTY))) {
                           dialog.setVisible(false);
                       }
                   }
               });
       
       dialog.pack();
       dialog.setVisible(true);
       
       String res = optionPane.getValue().toString();
       
       Call c = new Call(callId, accountId, sipUri, displayName);
              
       Vector params = new Vector();

       params.clear();
       params.add(new Integer(callId));

       if (res.equalsIgnoreCase("ACCEPT")) {
           ImageIcon img = new ImageIcon(gui.cl
                   .getResource("gui/resources/gray.gif"));
           if (c.name.trim().equalsIgnoreCase("")) {
               img.setDescription(sipUri);
           } else {
               img.setDescription(sipUri + " (" + c.name + ")");
           }
           gui.calls.add(c);
           gui.list2.addElement(img);
           gui.execute("core.acceptCall", params);
       } else if (res.equalsIgnoreCase("DECLINE")) {
           pendingCalls.add(c);
           gui.execute("core.endCall", params);
       } 
    }
}