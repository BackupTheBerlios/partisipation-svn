package test.events;

import java.awt.event.ActionEvent;
import test.Client;

public class Client_key_2_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

       public Client_key_2_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_2_actionPerformed(e);
        }
    }
