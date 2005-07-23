package gui.events;

import java.awt.event.WindowEvent;
import gui.Client;

/**
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */


public class Client_this_windowAdapter extends java.awt.event.WindowAdapter {
    Client adaptee;

   public Client_this_windowAdapter(Client adaptee) {
        this.adaptee = adaptee;
    }

    public void windowClosing(WindowEvent e) {
        adaptee.this_windowClosing(e);
    }
}
