package gui.events;

import java.awt.event.ActionEvent;
import gui.Client;


/**
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */


public class Client_key_0_ActionAdapter implements java.awt.event.ActionListener {
       Client adaptee;

       public Client_key_0_ActionAdapter(Client adaptee) {
           this.adaptee = adaptee;
       }

       public void actionPerformed(ActionEvent e) {
           adaptee.key_0_actionPerformed(e);
       }
   }
