package test.events;

import test.Client;
import java.awt.event.ActionEvent;

/**
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */



//  Menu item INFO pressed
public class Client_item_INFO_ActionAdapter implements
        java.awt.event.ActionListener {
   Client adaptee;

    public Client_item_INFO_ActionAdapter(Client adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.item_INFO_actionPerformed(e);
    }
}








