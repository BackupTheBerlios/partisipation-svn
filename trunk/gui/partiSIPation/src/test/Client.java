package test;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import org.apache.xmlrpc.XmlRpcClientLite;
import org.apache.xmlrpc.XmlRpcException;
import java.io.IOException;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.Vector;

/**
 * Simple client program with a graphical user interface. Makes remote procedure
 * calls and prints the server response into the text area.
 * 
 * @author Anton Huttenlocher
 * 
 */

public class Client extends JFrame {

    XmlRpcClientLite client;

    Vector params = new Vector();

    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

    JPanel contentPane;

    JPanel panel_REG = new JPanel();

    JMenuBar jMenuBar1 = new JMenuBar();

    JMenu jMenuFile = new JMenu();

    JMenu menu_HELP = new JMenu();

    JMenuItem jMenuFileExit = new JMenuItem();

    BorderLayout borderLayout1 = new BorderLayout();

    JMenuItem item_INFO = new JMenuItem();

    JMenuItem item_CONTENTS = new JMenuItem();

    JMenuItem menu_REGISTER = new JMenuItem();

    JPanel jLeft = new JPanel();

    GridBagLayout gridbag = new GridBagLayout();

    GridBagConstraints gbc = new GridBagConstraints();

    JScrollPane jScrollPane1 = new JScrollPane();

    JPanel panel_TOP = new JPanel();

    JButton key_DIAL = new JButton("   DIAL  ");

    JButton key_CANCEL = new JButton(" HANG UP");

    JButton key_0 = new JButton("0");

    JButton key_1 = new JButton("1");

    JButton key_2 = new JButton("2");

    JButton key_3 = new JButton("3");

    JButton key_4 = new JButton("4");

    JButton key_5 = new JButton("5");

    JButton key_6 = new JButton("6");

    JButton key_7 = new JButton("7");

    JButton key_8 = new JButton("8");

    JButton key_9 = new JButton("9");

    JButton key_SHARP = new JButton("*");

    JButton key_ASTERISK = new JButton("#");

    JTextArea jText = new JTextArea(10, 0);

    JLabel label_FULLNAME = new JLabel("Display Name:");

    JLabel label_USER = new JLabel("User Name:");

    JLabel label_PASSWORD = new JLabel("Password:");

    JLabel label_SIPDOMAIN = new JLabel("SIP Domain:");

    JTextField input_NUMBER = new JTextField(15);

    JTextField input_FULLNAME = new JTextField(15);

    JTextField input_SIPDOMAIN = new JTextField(15);

    JTextField input_USER = new JTextField(15);

    JPasswordField input_PASSWORD = new JPasswordField(15);

    JButton key_REG_CLEAR = new JButton("CLEAR");

    JButton key_REG_OK = new JButton("REGISTER");

    JOptionPane jDialog = new JOptionPane();

    JFrame frame_REG = new JFrame("Register");

    //Construct the frame
    public Client() {
        enableEvents(AWTEvent.WINDOW_EVENT_MASK);
        try {
            jbInit();
        } catch (Exception e) {
            // could not create GUI
        }
        try {
            client = new XmlRpcClientLite(InetAddress.getLocalHost()
                    .getHostName(), 7777);
        } catch (MalformedURLException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        } catch (UnknownHostException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
    }

    //Component initialization
    private void jbInit() throws Exception {
        contentPane = (JPanel) this.getContentPane();
        contentPane.setLayout(borderLayout1);
        this.setSize(new Dimension(400, 300));
        this.setTitle("partiSIPation");
        jMenuFile.setText("Options");
        menu_HELP.setText("Help");
        jMenuFileExit.setText("Exit");
        jMenuFileExit.addActionListener(new Client_jMenuFileExit_ActionAdapter(
                this));
        menu_REGISTER.setText("Register");
        item_INFO.setText("Info");
        item_CONTENTS.setText("Help Contents");
        menu_REGISTER.addActionListener(new Client_menu_REGISTER_ActionAdapter(
                this));
        jText.setEditable(false);

        key_1.addActionListener(new Client_key_1_ActionAdapter(this));
        key_2.addActionListener(new Client_key_2_ActionAdapter(this));
        key_3.addActionListener(new Client_key_3_ActionAdapter(this));
        key_4.addActionListener(new Client_key_4_ActionAdapter(this));
        key_5.addActionListener(new Client_key_5_ActionAdapter(this));
        key_6.addActionListener(new Client_key_6_ActionAdapter(this));
        key_7.addActionListener(new Client_key_7_ActionAdapter(this));
        key_8.addActionListener(new Client_key_8_ActionAdapter(this));
        key_9.addActionListener(new Client_key_9_ActionAdapter(this));
        key_0.addActionListener(new Client_key_0_ActionAdapter(this));

        key_DIAL.addActionListener(new Client_key_DIAL_actionAdapter(this));
        key_CANCEL.addActionListener(new Client_key_CANCEL_actionAdapter(this));
        key_REG_CLEAR.addActionListener(new Client_key_REG_CLEAR_ActionAdapter(
                this));
        key_REG_OK.addActionListener(new Client_key_REG_OK_ActionAdapter(this));
        item_INFO.addActionListener(new Client_item_INFO_ActionAdapter(this));

        jMenuFile.add(menu_REGISTER);
        jMenuFile.addSeparator();
        jMenuFile.add(jMenuFileExit);
        menu_HELP.add(item_CONTENTS);
        menu_HELP.addSeparator();
        menu_HELP.add(item_INFO);
        jMenuBar1.add(jMenuFile);
        jMenuBar1.add(menu_HELP);
        jLeft.setLayout(new GridLayout(0, 3));
        panel_TOP.setLayout(gridbag);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        jLeft.add(key_1);
        jLeft.add(key_2);
        jLeft.add(key_3);
        jLeft.add(key_4);
        jLeft.add(key_5);
        jLeft.add(key_6);
        jLeft.add(key_7);
        jLeft.add(key_8);
        jLeft.add(key_9);
        jLeft.add(key_ASTERISK);
        jLeft.add(key_0);
        jLeft.add(key_SHARP);

        gbc.weightx = 0.5;
        gbc.gridx = 0;
        gbc.gridy = 0;
        gridbag.setConstraints(key_CANCEL, gbc);
        panel_TOP.add(key_CANCEL);

        gbc.weightx = 5;
        gbc.gridx = 1;
        gbc.gridy = 0;
        gridbag.setConstraints(input_NUMBER, gbc);
        panel_TOP.add(input_NUMBER);

        gbc.weightx = 0.5;
        gbc.gridx = 2;
        gbc.gridy = 0;
        gridbag.setConstraints(key_DIAL, gbc);
        panel_TOP.add(key_DIAL);

        this.setJMenuBar(jMenuBar1);

        jScrollPane1.setPreferredSize(new Dimension(0, 100));
        jScrollPane1.getViewport().add(jText, null);

        this.getContentPane().add(panel_TOP, BorderLayout.NORTH);
        this.getContentPane().add(jLeft, BorderLayout.CENTER);
        this.getContentPane().add(jScrollPane1, BorderLayout.SOUTH);

        panel_REG.setLayout(new GridLayout(5, 2));
        panel_REG.add(label_FULLNAME);
        panel_REG.add(input_FULLNAME);
        panel_REG.add(label_USER);
        panel_REG.add(input_USER);
        panel_REG.add(label_PASSWORD);
        panel_REG.add(input_PASSWORD);
        panel_REG.add(label_SIPDOMAIN);
        panel_REG.add(input_SIPDOMAIN);
        panel_REG.add(key_REG_CLEAR);
        panel_REG.add(key_REG_OK);
        frame_REG.getContentPane().add(panel_REG, BorderLayout.CENTER);
        frame_REG.pack();
    }

    class Client_jMenuFileExit_ActionAdapter implements
            java.awt.event.ActionListener {
        Client adaptee;

        Client_jMenuFileExit_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.jMenuFileExit_actionPerformed(e);
        }
    }

    //File | Exit action performed
    public void jMenuFileExit_actionPerformed(ActionEvent e) {
        System.exit(0);
    }

    class Client_key_1_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_1_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_1_actionPerformed(e);
        }
    }

    class Client_key_2_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_2_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_2_actionPerformed(e);
        }
    }

    class Client_key_3_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_3_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_3_actionPerformed(e);
        }
    }

    class Client_key_4_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_4_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_4_actionPerformed(e);
        }
    }

    class Client_key_5_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_5_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_5_actionPerformed(e);
        }
    }

    class Client_key_6_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_6_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_6_actionPerformed(e);
        }
    }

    class Client_key_7_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_7_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_7_actionPerformed(e);
        }
    }

    class Client_key_8_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_8_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_8_actionPerformed(e);
        }
    }

    class Client_key_9_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_9_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_9_actionPerformed(e);
        }
    }

    class Client_key_0_ActionAdapter implements java.awt.event.ActionListener {
        Client adaptee;

        Client_key_0_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_0_actionPerformed(e);
        }
    }

    class Client_key_REG_CLEAR_ActionAdapter implements
            java.awt.event.ActionListener {
        Client adaptee;

        Client_key_REG_CLEAR_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_REG_CLEAR_actionPerformed(e);
        }
    }

    class Client_menu_REGISTER_ActionAdapter implements
            java.awt.event.ActionListener {
        Client adaptee;

        Client_menu_REGISTER_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.menu_REGISTER_actionPerformed(e);
        }
    }

    // Button REGISTER pressed
    class Client_key_REG_OK_ActionAdapter implements
            java.awt.event.ActionListener {
        Client adaptee;

        Client_key_REG_OK_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_REG_OK_actionPerformed(e);
        }
    }

    //  Menu item INFO pressed
    class Client_item_INFO_ActionAdapter implements
            java.awt.event.ActionListener {
        Client adaptee;

        Client_item_INFO_ActionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.item_INFO_actionPerformed(e);
        }
    }

    // Menu item INFO clicked
    public void item_INFO_actionPerformed(ActionEvent e) {
        ClassLoader cl = this.getClass().getClassLoader();
        URL logoURL = cl.getResource("test/ressources/logo.jpg");
        ImageIcon icon = new ImageIcon(logoURL);
        jDialog.showMessageDialog(this, "", "Info",
                JOptionPane.INFORMATION_MESSAGE, icon);
    }

    // Button REGISTER pressed
    public void key_REG_OK_actionPerformed(ActionEvent e) {
        params.clear();
        params.addElement(input_FULLNAME.getText());
        params.addElement(input_USER.getText());
        params.addElement(input_SIPDOMAIN.getText());
        params.addElement(new String(input_PASSWORD.getPassword()));
        try {
            jText
                    .append((String) client.execute("sip.register", params)
                            + "\n");
        } catch (XmlRpcException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        } catch (IOException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
    }

    // BUTTON 1 PRESSED ACTION
    public void key_1_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "1");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_2_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "2");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_3_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "3");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_4_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "4");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_5_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "5");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_6_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "6");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_7_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "7");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_8_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "8");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_9_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "9");
    }

    //BUTTON 1 PRESSED ACTION
    public void key_0_actionPerformed(ActionEvent e) {
        input_NUMBER.setText(input_NUMBER.getText() + "0");
    }

    // BUTTON "CLEAR" PRESSED ACTION
    public void key_REG_CLEAR_actionPerformed(ActionEvent e) {
        input_FULLNAME.setText("");
        input_USER.setText("");
        input_PASSWORD.setText("");
        input_SIPDOMAIN.setText("");
        input_FULLNAME.grabFocus();
    }

    //Help | About action performed
    //Overridden so we can exit when window is closed
    protected void processWindowEvent(WindowEvent e) {
        super.processWindowEvent(e);
        if (e.getID() == WindowEvent.WINDOW_CLOSING) {
            jMenuFileExit_actionPerformed(null);
        }
    }

    // ##############################
    class Client_key_DIAL_actionAdapter implements
            java.awt.event.ActionListener {
        Client adaptee;

        Client_key_DIAL_actionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_DIAL_actionPerformed(e);
        }
    }

    void key_DIAL_actionPerformed(ActionEvent e) {
        params.clear();
        params.addElement(input_NUMBER.getText());
        try {
            jText
                    .append((String) client.execute("sip.makeCall", params)
                            + "\n");
        } catch (XmlRpcException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        } catch (IOException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
    }

    class Client_key_CANCEL_actionAdapter implements
            java.awt.event.ActionListener {
        Client adaptee;

        Client_key_CANCEL_actionAdapter(Client adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.key_CANCEL_actionPerformed(e);
        }
    }

    void key_CANCEL_actionPerformed(ActionEvent e) {
        params.clear();
        try {
            jText.append((String) client.execute("sip.endCall", params) + "\n");
        } catch (XmlRpcException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        } catch (IOException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        /* 
         */
    }

    void menu_REGISTER_actionPerformed(ActionEvent e) {
        Dimension screen_size = Toolkit.getDefaultToolkit().getScreenSize();
        Dimension frame_REG_size = frame_REG.getSize();
        if (frame_REG_size.height > screenSize.height) {
            frame_REG_size.height = screenSize.height;
        }
        if (frame_REG_size.width > screenSize.width) {
            frame_REG_size.width = screenSize.width;
        }
        frame_REG.setLocation((screenSize.width - frame_REG_size.width) / 2,
                (screenSize.height - frame_REG_size.height) / 2);
        frame_REG.setVisible(true);
    }

    //Main method
    public static void main(String[] args) {
        try {
            UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
        } catch (Exception e) {
            e.printStackTrace();
        }

        boolean packFrame = false;

        Client frame = new Client();
        //Validate frames that have preset sizes
        //Pack frames that have useful preferred size info, e.g. from their
        // layout
        if (packFrame) {
            frame.pack();
        } else {
            frame.validate();
        }
        //Center the window
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        Dimension frameSize = frame.getSize();
        if (frameSize.height > screenSize.height) {
            frameSize.height = screenSize.height;
        }
        if (frameSize.width > screenSize.width) {
            frameSize.width = screenSize.width;
        }
        frame.setLocation((screenSize.width - frameSize.width) / 2,
                (screenSize.height - frameSize.height) / 2);
        frame.setVisible(true);

    }
}