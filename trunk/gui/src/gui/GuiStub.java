package gui;

import java.util.Enumeration;
import java.util.Vector;

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

    private Gui gui;
    private ClassLoader cl;
    private Vector pendingCalls = new Vector();
    private Vector params = new Vector();

    public GuiStub(Gui g) {
        gui = g;
        cl = gui.cl;
    }

    public boolean changeRegStatus(int accountId, boolean registered) {
        Enumeration e = gui.accounts.elements();
        int i = 0;
        while (e.hasMoreElements()) {
            Integer n = (Integer) e.nextElement();
            if (n.intValue() == accountId) {
                if (registered) {
                    gui.list1.set(i, "#"+accountId+": registered");
                }
                else gui.list1.set(i, "#"+accountId+": not registered");
                break;
            }
            i++;
        }
        return true;
    }

    public boolean changeCallStatus(int callId, String callStatus) {
        if (callStatus.equalsIgnoreCase("ACCEPTED")) {
            
            Call c = getCall(callId, gui.calls);
          	if (c == null) {
          	    // accepted by user: add call to the list 
          	    c = getCall(callId, pendingCalls); 
          	    //int index = getIndex(c.id, pendingCalls);
          	    removeCall(callId,pendingCalls);
          	    gui.calls.addElement(c);
          	    gui.list2.addElement("["+callId+"] : <ACC> : "+c.sipurl);
          	} else {
          	    // accepted by other side: update call status
          	    int index = getIndex(c.id, gui.calls);
          	    gui.list2.removeElementAt(index);
          	    gui.list2.insertElementAt("["+callId+"] : <ACC> : "+getCall(callId, gui.calls).sipurl, index);
          	}        	
          	
            
       /*     Call nc = getCall(callId, gui.calls); //pendingCalls);
            int index = getIndex(nc.id, gui.calls); //pendingCalls);
 
       //     gui.list2.removeElementAt(index);
            gui.calls.add(nc);
       //     pendingCalls.remove(index);
            //gui.list2.insertElementAt("["+callId+"] : <ACC> : "+getCall(callId, gui.calls).sipurl, index);
            
            gui.list2.removeElementAt(index);
            gui.list2.insertElementAt("["+callId+"] : <ACC> : "+getCall(callId, gui.calls).sipurl, index);
          //  gui.list2.addElement("["+callId+"] : <ACC> : "+getCall(callId, gui.calls).sipurl);
            
            */
            
            print("Call # "+callId+" accepted.");
            
         } else if (callStatus.equalsIgnoreCase("TERMINATED")) {
             
        	if (getCall(callId, pendingCalls) == null) { // hang-up-termination by the other side
        	    Call c = getCall(callId, gui.calls);
              	int index = getIndex(c.id, gui.calls);
        	    gui.list2.remove(index);
        	} else { // decline-termination by user
        	    removeCall(callId, pendingCalls);
        	}
        	print("Call # " + callId +" terminated.");
        }  
        else {
            
          	Call c = getCall(callId, gui.calls);
          	if (c == null) {
          	    c = getCall(callId, pendingCalls);   	  
          	}
          	int index = getIndex(c.id, gui.calls);
          	if (index == -1) {
          	    index = getIndex(c.id, pendingCalls);
          	}

       if (callStatus.equalsIgnoreCase("TRYING")) {           
            gui.list2.removeElementAt(index);
            gui.list2.insertElementAt("["+callId+"] : <TRY> : "+getCall(callId, gui.calls).sipurl, index);
    //       gui.calls.add(c);
      //     gui.list2.addElement("["+callId+"] : <TRY> : "+getCall(callId, gui.calls).sipurl); //, index);
    //       pendingCalls.remove(index);
        } else if (callStatus.equalsIgnoreCase("RINGING")) {
            gui.list2.removeElementAt(index);
            gui.list2.insertElementAt("["+callId+"] : <RIN> : "+getCall(callId, gui.calls).sipurl, index);
        } else if (callStatus.equalsIgnoreCase("DECLINED")) {
            removeCall(callId, gui.calls);
            print("Call # " + callId +" declined.");
        } else if (callStatus.equalsIgnoreCase("CANCELLED")) {
            removeCall(callId, gui.calls);
            gui.list2.remove(index);
            print("Call # " + callId +" cancelled.");
        } else if (callStatus.equalsIgnoreCase("UNREACHABLE")) {
            removeCall(callId, gui.calls);
            print("Call # " + callId +" unreachable.");
        } 
        
        }
        return true;
    }

    public boolean showUserEvent(int accountId, String category, String title,
            String message, String details) {
        if (category.equalsIgnoreCase("DEBUG")) {
            JOptionPane.showMessageDialog(gui, message+"\n\n"+details, title,
                    JOptionPane.QUESTION_MESSAGE);
        } else if (category.equalsIgnoreCase("INFO")) {
            JOptionPane.showMessageDialog(gui, message+"\n\n"+details, title,
                    JOptionPane.INFORMATION_MESSAGE);
        } else if (category.equalsIgnoreCase("MESSAGE")) {
            JOptionPane.showMessageDialog(gui, message+"\n\n"+details, title,
                    JOptionPane.PLAIN_MESSAGE);
        } else if (category.equalsIgnoreCase("WARNING")){
            JOptionPane.showMessageDialog(gui, message+"\n\n"+details, title,
                    JOptionPane.WARNING_MESSAGE);
        } else {
            JOptionPane.showMessageDialog(gui, message+"\n\n"+details, title,
                    JOptionPane.ERROR_MESSAGE);
        }
        return true;
    }

    public boolean registerCore() {
        print("Core registered.");
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

        Call c = new Call(callId, accountId, sipUri, displayName);
        pendingCalls.add(c);
        print("Incoming call # "+ callId + " from "+ sipUri + " ("+ displayName+")"); 
        
        params.clear();
        params.add(new Integer(callId));
        
        if (n == 0) {
            gui.execute("core.acceptCall", params);
        } else {
            gui.execute("core.endCall", params);
        }
        return true;
    }

    public boolean setSpeakerVolume(double level) {
        gui.jSlider1.setValue((int)(level * 100));
        return true;
    }

    public boolean setMicroVolume(double level) {
        gui.jSlider2.setValue((int)(level * 100));
        return true;
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
    
    /**
     * 
     * @param id
     * @param vec
     */
    private void removeCall(int id, Vector vec) {
        Enumeration e = vec.elements();
        int i = 0;
        while (e.hasMoreElements()) {
            Call c = (Call) e.nextElement();
            if (c.id == id) {
                vec.remove(i);
            }
            i++;
        }
        return;
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
    
    private void print(String s) {
        gui.jTextArea1.append(Utils.getTimestamp()+": "+s+"\n");
    }
    
    }

