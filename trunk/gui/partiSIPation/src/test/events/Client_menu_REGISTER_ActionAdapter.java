package test.events;

import test.Client;
import java.awt.event.ActionEvent;

/**
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */


public class Client_menu_REGISTER_ActionAdapter implements
           java.awt.event.ActionListener {
       Client adaptee;

       public Client_menu_REGISTER_ActionAdapter(Client adaptee) {
           this.adaptee = adaptee;
       }

       public void actionPerformed(ActionEvent e) {
           adaptee.menu_REGISTER_actionPerformed(e);
       }
   }
