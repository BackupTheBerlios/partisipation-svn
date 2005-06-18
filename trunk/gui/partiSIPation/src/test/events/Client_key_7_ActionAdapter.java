package test.events;

import test.Client;
import java.awt.event.ActionEvent;

/**
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */


public class Client_key_7_ActionAdapter implements java.awt.event.ActionListener {
    Client adaptee;

   public Client_key_7_ActionAdapter(Client adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.key_7_actionPerformed(e);
    }
}
