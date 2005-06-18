package test.events;

import java.awt.event.ActionEvent;
import test.Client;

/**
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */


public class Client_key_REG_CLEAR_ActionAdapter implements
     java.awt.event.ActionListener {
 Client adaptee;

 public Client_key_REG_CLEAR_ActionAdapter(Client adaptee) {
     this.adaptee = adaptee;
 }

 public void actionPerformed(ActionEvent e) {
     adaptee.key_REG_CLEAR_actionPerformed(e);
 }
}
