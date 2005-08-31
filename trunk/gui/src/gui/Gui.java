package gui;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.util.Vector;
import javax.swing.DefaultListModel;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
//import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.WindowConstants;
import org.apache.xerces.parsers.SAXParser;
import org.apache.xmlrpc.WebServer;
import org.apache.xmlrpc.XmlRpc;
import org.apache.xmlrpc.XmlRpcClientLite;
import org.netbeans.lib.awtextra.AbsoluteConstraints;
import org.netbeans.lib.awtextra.AbsoluteLayout;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import java.util.Enumeration;

/**
 * This is the graphical user interface designed with Java Swing.
 *
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */
public class Gui extends JFrame {

    private int slidervalue = 50;

    private String accfile = "";

    // client part - sends calls to core
    private XmlRpcClientLite client;

    // server part - processes calls from core
    private WebServer server;

    // stores the parameters for a call
    private Vector params = new Vector();

    // stores the list of the phone numbers
    private Vector phonebook = new Vector();

    // account to use for current call
    private Account actacc = null;

    // current callee's display name
    private String dname = "";

    /**
     * This method creates the GUI window and the XML-RPC part.
     */
    public Gui() {
        initComponents();

        try {

            accfile = System.getProperty("user.home", ".") + "/phonebook.xml";

            client = new XmlRpcClientLite(InetAddress.getByName(Utils.COREHOST)
                    .getHostName(), Utils.COREPORT);

            //XmlRpc.setDebug(true);
            server = new WebServer(Utils.GUIPORT, InetAddress
                    .getByName(Utils.GUIHOST));

            GuiStub guiStub = new GuiStub(this);

            server.start();
            server.addHandler("gui", guiStub);

            // call REGISTER_GUI immediately after GUI has been opened
            params.clear();
            params.addElement(Utils.GUIHOST);
            params.addElement(new Integer(Utils.GUIPORT));

            Object o = execute("core.registerGui", params, 3000);
            if (o == null) {
                // --- something
            } else if (((String) o).equalsIgnoreCase("OK")) {
                print("GUI registered.");

                params.clear();
                o = execute("core.accountGetAll", params);

                int a = 0;
                if (o != null && o instanceof Vector) {
                    Vector v = (Vector) o;
                    Enumeration e = v.elements();
                    while (e.hasMoreElements()) {
                        Integer n = (Integer) e.nextElement();
                        list1.addElement("#" + n.toString()
                                + ": not registered");
                        Account acc = new Account(n.intValue(), false);
                        accounts.add(acc);
                        a++;
                    }
                    print("Got " + a + " accounts.");
                }
            } else {
                print("ERROR: GUI not registered.");
            }

            readPhonebook();
            Enumeration enum = phonebook.elements();
            while (enum.hasMoreElements()) {
                Contact con = (Contact) enum.nextElement();
                list3.addElement(con.name);
            }

        } catch (MalformedURLException e1) {
            e1.printStackTrace();
        } catch (UnknownHostException e1) {
            e1.printStackTrace();
        }
    }

    /**
     * This method initializes all SWING components of the GUI window.
     */
    private void initComponents() {
        jTabbedPane1 = new JTabbedPane();
        jPanel1 = new JPanel();
        jPanel5 = new JPanel();
        jTextField1 = new JTextField();
        jLabel1 = new JLabel();
        jLabel2 = new JLabel();
        jLabel3 = new JLabel();
        jLabel4 = new JLabel();
        jLabel5 = new JLabel();
        jLabel6 = new JLabel();
        jLabel7 = new JLabel();
        jLabel8 = new JLabel();
        jLabel9 = new JLabel();
        jLabel10 = new JLabel();
        jLabel11 = new JLabel();
        jLabel12 = new JLabel();
        jLabel13 = new JLabel();
        jLabel14 = new JLabel();
        jLabel15 = new JLabel();
        jLabel16 = new JLabel();
        jPanel3 = new JPanel();
        jPanel4 = new JPanel();
        jSlider1 = new JSlider(0, 100, 50);
        jSlider2 = new JSlider(0, 100, 50);
        menuBar = new JMenuBar();
        helpMenu = new JMenu();
        contentsMenuItem = new JMenuItem();
        aboutMenuItem = new JMenuItem();
        jMenuItem1 = new JMenuItem();
        jMenuItem2 = new JMenuItem();
        jScrollPane1 = new JScrollPane();
        jScrollPane2 = new JScrollPane();
        jScrollPane3 = new JScrollPane();
        jScrollPane4 = new JScrollPane();
        list1 = new DefaultListModel();
        list2 = new DefaultListModel();
        list3 = new DefaultListModel();
        jList1 = new JList(list1);
        jList2 = new JList(list2);
        jList3 = new JList(list3);
        jLabel17 = new JLabel();
        jLabel18 = new JLabel();
        jLabel19 = new JLabel();
        jLabel20 = new JLabel();
        jLabel21 = new JLabel();
        jLabel22 = new JLabel();
        jTextField4 = new JTextField();
        jLabel23 = new JLabel();
        jLabel24 = new JLabel();
        jLabel27 = new JLabel();
        jLabel28 = new JLabel();
        jLabel29 = new JLabel();
        jLabel30 = new JLabel();
        jTextField2 = new JTextField();
        jTextField3 = new JTextField();
        jTextField5 = new JTextField();
        jPasswordField1 = new JPasswordField();
        jTextField6 = new JTextField();
        jTextField7 = new JTextField();
        jTextField8 = new JTextField();
        jTextField9 = new JTextField();
        jTextField10 = new JTextField();
        jLabel25 = new JLabel();
        jLabel26 = new JLabel();
        jCheckBox1 = new JCheckBox();
        jButton1 = new JButton();
        jButton2 = new JButton();
        jButton3 = new JButton();
        jButton4 = new JButton();
        jButton5 = new JButton();
        jButton6 = new JButton();
        jButton7 = new JButton();
        jButton8 = new JButton();
        jButton9 = new JButton();
        jTextArea1 = new JTextArea();
        addressBook = new JFrame();
        accounts = new Vector();
        calls = new Vector();

        getContentPane().setLayout(new AbsoluteLayout());

        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        jPanel1.setLayout(new AbsoluteLayout());

        jPanel1.add(jTextField1, new AbsoluteConstraints(170, 10, 410, -1));

        cl = this.getClass().getClassLoader();

        jLabel1.setIcon(new ImageIcon(cl.getResource("gui/resources/1.gif")));
        jPanel1.add(jLabel1, new AbsoluteConstraints(10, 10, -1, -1));

        jLabel2.setIcon(new ImageIcon(cl.getResource("gui/resources/2.gif")));
        jPanel1.add(jLabel2, new AbsoluteConstraints(60, 10, -1, -1));

        jLabel3.setIcon(new ImageIcon(cl.getResource("gui/resources/3.gif")));
        jPanel1.add(jLabel3, new AbsoluteConstraints(110, 10, -1, -1));

        jLabel4.setIcon(new ImageIcon(cl.getResource("gui/resources/4.gif")));
        jPanel1.add(jLabel4, new AbsoluteConstraints(10, 60, -1, -1));

        jLabel5.setIcon(new ImageIcon(cl.getResource("gui/resources/5.gif")));
        jPanel1.add(jLabel5, new AbsoluteConstraints(60, 60, -1, -1));

        jLabel6.setIcon(new ImageIcon(cl.getResource("gui/resources/6.gif")));
        jPanel1.add(jLabel6, new AbsoluteConstraints(110, 60, -1, -1));

        jLabel7.setIcon(new ImageIcon(cl.getResource("gui/resources/7.gif")));
        jPanel1.add(jLabel7, new AbsoluteConstraints(10, 110, -1, -1));

        jLabel8.setIcon(new ImageIcon(cl.getResource("gui/resources/8.gif")));
        jPanel1.add(jLabel8, new AbsoluteConstraints(60, 110, -1, -1));

        jLabel9.setIcon(new ImageIcon(cl.getResource("gui/resources/9.gif")));
        jPanel1.add(jLabel9, new AbsoluteConstraints(110, 110, -1, -1));

        jLabel10.setIcon(new ImageIcon(cl
                .getResource("gui/resources/asterisk.gif")));
        jPanel1.add(jLabel10, new AbsoluteConstraints(10, 160, -1, -1));

        jLabel11.setIcon(new ImageIcon(cl.getResource("gui/resources/0.gif")));
        jPanel1.add(jLabel11, new AbsoluteConstraints(60, 160, -1, -1));

        jLabel12.setIcon(new ImageIcon(cl
                .getResource("gui/resources/sharp.gif")));
        jPanel1.add(jLabel12, new AbsoluteConstraints(110, 160, -1, -1));

        jLabel13.setIcon(new ImageIcon(cl
                .getResource("gui/resources/phone_up.gif")));
        jLabel13.setVerticalAlignment(SwingConstants.TOP);
        jLabel13.setPreferredSize(new java.awt.Dimension(54, 54));
        jPanel1.add(jLabel13, new AbsoluteConstraints(590, 10, 54, 54));

        jLabel14.setIcon(new ImageIcon(cl
                .getResource("gui/resources/phone_down_dis.gif")));
        jLabel14.setVerticalAlignment(SwingConstants.TOP);
        jLabel14.setPreferredSize(new java.awt.Dimension(54, 54));
        jPanel1.add(jLabel14, new AbsoluteConstraints(590, 150, 54, 54));

        jLabel15.setIcon(new ImageIcon(cl
                .getResource("gui/resources/addressbook.gif")));
        jLabel15.setVerticalAlignment(SwingConstants.TOP);
        jLabel15.setPreferredSize(new java.awt.Dimension(54, 54));
        jPanel1.add(jLabel15, new AbsoluteConstraints(590, 80, 54, 54));

        jScrollPane2.setAutoscrolls(true);
        jTextArea1.setBorder(new javax.swing.border.EmptyBorder(
                new java.awt.Insets(1, 1, 1, 1)));
        jScrollPane2.setViewportView(jTextArea1);

        jPanel1.add(jScrollPane2, new AbsoluteConstraints(10, 220, 640, 120));

        jList2.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        jScrollPane3.setAutoscrolls(true);
        jList2.setBorder(new javax.swing.border.EmptyBorder(
                new java.awt.Insets(1, 1, 1, 1)));
        jList2
                .setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jList2.setValueIsAdjusting(true);
        CallListCellRenderer cscr = new CallListCellRenderer();

        jList2.setCellRenderer(cscr);

        jScrollPane3.setViewportView(jList2);

        jPanel1.add(jScrollPane3, new AbsoluteConstraints(170, 40, 410, 170));

        jTabbedPane1.addTab("Calls", jPanel1);

        jPanel3.setLayout(new AbsoluteLayout());

        jTabbedPane1.addTab("Accounts", jPanel3);
        jList1.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        jScrollPane1.setAutoscrolls(true);
        jList1.setBorder(new javax.swing.border.EmptyBorder(
                new java.awt.Insets(1, 1, 1, 1)));
        jList1
                .setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jList1.setValueIsAdjusting(true);
        jScrollPane1.setViewportView(jList1);

        jPanel3.add(jTextField8, new AbsoluteConstraints(350, 20, 200, -1));

        jPanel3.add(jTextField2, new AbsoluteConstraints(350, 50, 200, -1));

        jPanel3.add(jTextField3, new AbsoluteConstraints(350, 80, 200, -1));

        jLabel17.setText("Account Name:");
        jPanel3.add(jLabel17, new AbsoluteConstraints(210, 20, 110, -1));

        jLabel18.setText("User Name:");
        jPanel3.add(jLabel18, new AbsoluteConstraints(210, 80, 110, -1));

        jLabel19.setText("Domain:");
        jPanel3.add(jLabel19, new AbsoluteConstraints(210, 140, 80, -1));

        jLabel20.setText("Outbound Proxy:");
        jPanel3.add(jLabel20, new AbsoluteConstraints(210, 200, -1, -1));

        jLabel21.setText("Display Name:");
        jPanel3.add(jLabel21, new AbsoluteConstraints(210, 50, -1, -1));

        jLabel22.setText("Authentication Name:");
        jPanel3.add(jLabel22, new AbsoluteConstraints(210, 110, -1, -1));

        jPanel3.add(jTextField4, new AbsoluteConstraints(350, 110, 200, -1));

        jLabel23.setText("Password:");
        jPanel3.add(jLabel23, new AbsoluteConstraints(210, 170, -1, -1));

        jLabel24.setText("Registrar:");
        jPanel3.add(jLabel24, new AbsoluteConstraints(210, 230, -1, -1));

        jPanel3.add(jTextField5, new AbsoluteConstraints(350, 140, 200, -1));

        jPasswordField1.setPreferredSize(new java.awt.Dimension(11, 19));
        jPanel3
                .add(jPasswordField1,
                        new AbsoluteConstraints(350, 170, 200, -1));

        jPanel3.add(jTextField6, new AbsoluteConstraints(350, 200, 200, -1));

        jPanel3.add(jTextField7, new AbsoluteConstraints(350, 230, 200, -1));

        jButton1.setEnabled(false);
        jButton3.setEnabled(false);
        jButton4.setEnabled(false);

        jLabel25.setText("Auto-Register:");
        jPanel3.add(jLabel25, new AbsoluteConstraints(210, 260, -1, -1));

        jPanel3.add(jCheckBox1, new AbsoluteConstraints(350, 260, -1, -1));

        jButton1.setText("(Un)Register");
        jPanel3.add(jButton1, new AbsoluteConstraints(330, 300, 110, -1));

        jButton2.setText("Save");
        jPanel3.add(jButton4, new AbsoluteConstraints(440, 300, 110, -1));

        jButton3.setText("Set Values");
        jPanel3.add(jButton3, new AbsoluteConstraints(550, 300, 110, -1));

        jButton4.setText("Delete");
        jPanel3.add(jButton2, new AbsoluteConstraints(230, 300, 100, -1));

        jButton5.setText("Create");
        jPanel3.add(jButton5, new AbsoluteConstraints(120, 300, 110, -1));

        jButton6.setText("Clear");
        jPanel3.add(jButton6, new AbsoluteConstraints(10, 300, 110, -1));

        jPanel3.add(jScrollPane1, new AbsoluteConstraints(10, 10, 190, 260));

        jPanel4.setLayout(new AbsoluteLayout());

        jPanel4.add(jSlider1, new AbsoluteConstraints(140, 80, 420, -1));

        jPanel4.add(jSlider2, new AbsoluteConstraints(100, 220, 420, -1));

        jLabel27.setIcon(new ImageIcon(cl
                .getResource("gui/resources/speaker.gif")));
        jPanel4.add(jLabel27, new AbsoluteConstraints(20, 40, 100, 100));

        jLabel28.setIcon(new ImageIcon(cl
                .getResource("gui/resources/micro.gif")));
        jPanel4.add(jLabel28, new AbsoluteConstraints(540, 180, 100, 100));

        jLabel16.setFont(new java.awt.Font("Tahoma", 1, 24));
        jLabel16.setText(" 50%");
        jPanel4.add(jLabel16,
                new org.netbeans.lib.awtextra.AbsoluteConstraints(570, 70, -1,
                        -1));

        jLabel26.setFont(new java.awt.Font("Tahoma", 1, 24));
        jLabel26.setText(" 50%");
        jPanel4.add(jLabel26,
                new org.netbeans.lib.awtextra.AbsoluteConstraints(20, 210, -1,
                        -1));

        jTabbedPane1.addTab("Options", jPanel4);

        getContentPane().add(jTabbedPane1,
                new AbsoluteConstraints(0, 0, 670, 380));
        this.setResizable(false);

        // Address book window initialization
        addressBook.getContentPane().setLayout(new AbsoluteLayout());

        jPanel5.setLayout(new AbsoluteLayout());

        jLabel29.setText("Display name:");
        jPanel5.add(jLabel29, new AbsoluteConstraints(10, 20, -1, -1));

        jPanel5.add(jTextField9, new AbsoluteConstraints(115, 20, 165, -1));

        jLabel30.setText("Phone / SIP URL:");
        jPanel5.add(jLabel30, new AbsoluteConstraints(10, 50, -1, -1));

        jPanel5.add(jTextField10, new AbsoluteConstraints(115, 50, 165, -1));

        jList3
                .setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jList3.setValueIsAdjusting(true);
        jScrollPane4.setViewportView(jList3);

        jPanel5.add(jScrollPane4, new AbsoluteConstraints(10, 80, 270, 150));

        jButton7.setText("Select");
        jPanel5.add(jButton7, new AbsoluteConstraints(10, 240, 90, -1));

        jButton8.setText("Create");
        jPanel5.add(jButton8, new AbsoluteConstraints(100, 240, 90, -1));

        jButton9.setText("Delete");
        jPanel5.add(jButton9, new AbsoluteConstraints(190, 240, 90, -1));

        addressBook.getContentPane().add(jPanel5,
                new AbsoluteConstraints(0, 0, 300, 280));
        addressBook.setResizable(false);
        addressBook.pack();

        helpMenu.setText("Help");
        helpMenu.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                helpMenuActionPerformed(evt);
            }
        });

        contentsMenuItem.setText("Contents");
        helpMenu.add(contentsMenuItem);

        aboutMenuItem.setText("About");
        helpMenu.add(aboutMenuItem);

        jMenuItem1.setText("Item");
        helpMenu.add(jMenuItem1);

        jMenuItem2.setText("Item");
        helpMenu.add(jMenuItem2);

        menuBar.add(helpMenu);

        setJMenuBar(menuBar);

        jList1
                .addListSelectionListener(new javax.swing.event.ListSelectionListener() {
                    public void valueChanged(
                            javax.swing.event.ListSelectionEvent evt) {
                        jList1ValueChanged(evt);
                    }
                });

        jList2
                .addListSelectionListener(new javax.swing.event.ListSelectionListener() {
                    public void valueChanged(
                            javax.swing.event.ListSelectionEvent evt) {
                        jList2ValueChanged(evt);
                    }
                });

        jList3
                .addListSelectionListener(new javax.swing.event.ListSelectionListener() {
                    public void valueChanged(
                            javax.swing.event.ListSelectionEvent evt) {
                        jList3ValueChanged(evt);
                    }
                });

        jLabel1.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel1MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel1MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel1MouseClicked(evt);
            }
        });

        jLabel2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel2MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel2MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel2MouseClicked(evt);
            }
        });

        jLabel3.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel3MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel3MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel3MouseClicked(evt);
            }
        });

        jLabel4.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel4MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel4MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel4MouseClicked(evt);
            }
        });

        jLabel5.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel5MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel5MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel5MouseClicked(evt);
            }
        });

        jLabel6.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel6MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel6MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel6MouseClicked(evt);
            }
        });

        jLabel7.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel7MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel7MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel7MouseClicked(evt);
            }
        });

        jLabel8.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel8MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel8MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel8MouseClicked(evt);
            }
        });

        jLabel9.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel9MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel9MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel9MouseClicked(evt);
            }
        });

        jLabel10.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel10MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel10MouseReleased(evt);
            }
        });

        jLabel11.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel11MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel11MouseReleased(evt);
            }

            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel11MouseClicked(evt);
            }
        });

        jLabel12.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel12MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel12MouseReleased(evt);
            }
        });

        jLabel13.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel13MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel13MouseReleased(evt);
            }
        });

        jLabel14.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel14MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel14MouseReleased(evt);
            }
        });

        jLabel15.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jLabel15MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jLabel15MouseReleased(evt);
            }
        });

        jButton1.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton1MouseClicked(evt);
            }
        });

        jButton2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton2MouseClicked(evt);
            }
        });

        jButton3.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton3MouseClicked(evt);
            }
        });

        jButton4.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton4MouseClicked(evt);
            }
        });

        jButton5.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton5MouseClicked(evt);
            }
        });

        jButton6.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton6MouseClicked(evt);
            }
        });

        jButton7.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton7MouseClicked(evt);
            }
        });

        jButton8.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton8MouseClicked(evt);
            }
        });

        jButton9.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButton9MouseClicked(evt);
            }
        });

        jSlider1.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSlider1StateChanged(evt);
            }
        });
        jSlider1.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jSlider1MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jSlider1MouseReleased(evt);
            }
        });

        jSlider2.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSlider2StateChanged(evt);
            }
        });
        jSlider2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jSlider2MousePressed(evt);
            }

            public void mouseReleased(java.awt.event.MouseEvent evt) {
                jSlider2MouseReleased(evt);
            }
        });

        pack();
    }

    // </editor-fold>

    public void jComboBox1ActionPerformed(java.awt.event.ActionEvent evt) {
        // TODO add your handling code here:
    }

    public void helpMenuActionPerformed(java.awt.event.ActionEvent evt) {
        // TODO add your handling code here:
    }

    /**
     * Main method. It calls methods to create the GUI window and to make it
     * visible to the user.
     *
     * @param args
     *            Console arguments. They all will be ignored.
     */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new Gui().setVisible(true);
            }
        });
    }

    // Variables declaration - do not modify
    public JMenuItem aboutMenuItem;

    public JMenuItem contentsMenuItem;

    public JMenu helpMenu;

    public JTextField jTextField1;

    public JLabel jLabel1;

    public JLabel jLabel10;

    public JLabel jLabel11;

    public JLabel jLabel12;

    public JLabel jLabel13;

    public JLabel jLabel14;

    public JLabel jLabel15;

    public JLabel jLabel16;

    public JLabel jLabel17;

    public JLabel jLabel18;

    public JLabel jLabel19;

    public JLabel jLabel2;

    public JLabel jLabel20;

    public JLabel jLabel21;

    public JLabel jLabel22;

    public JLabel jLabel23;

    public JLabel jLabel24;

    public JLabel jLabel25;

    public JLabel jLabel26;

    public JLabel jLabel27;

    public JLabel jLabel28;

    public JLabel jLabel29;

    public JLabel jLabel30;

    public JLabel jLabel3;

    public JLabel jLabel4;

    public JLabel jLabel5;

    public JLabel jLabel6;

    public JLabel jLabel7;

    public JLabel jLabel8;

    public JLabel jLabel9;

    public JMenuItem jMenuItem1;

    public JMenuItem jMenuItem2;

    public JPanel jPanel1;

    public JPanel jPanel3;

    public JPanel jPanel4;

    public JPanel jPanel5;

    public JSlider jSlider1;

    public JSlider jSlider2;

    public JTabbedPane jTabbedPane1;

    public JMenuBar menuBar;

    public JList jList1;

    public JList jList2;

    public JList jList3;

    public ClassLoader cl;

    public DefaultListModel list1;

    public DefaultListModel list2;

    public DefaultListModel list3;

    public JScrollPane jScrollPane1;

    public JScrollPane jScrollPane2;

    public JScrollPane jScrollPane3;

    public JScrollPane jScrollPane4;

    public JTextField jTextField2;

    public JTextField jTextField3;

    public JTextField jTextField4;

    public JTextField jTextField5;

    public JTextField jTextField6;

    public JTextField jTextField7;

    public JTextField jTextField8;

    public JTextField jTextField9;

    public JTextField jTextField10;

    public JPasswordField jPasswordField1;

    public JButton jButton1;

    public JButton jButton2;

    public JButton jButton3;

    public JButton jButton4;

    public JButton jButton5;

    public JButton jButton6;

    public JButton jButton7;

    public JButton jButton8;

    public JButton jButton9;

    public JCheckBox jCheckBox1;

    public JTextArea jTextArea1;

    public Vector accounts;

    public Vector calls;

    public JFrame addressBook;

    // End of variables declaration

    public void jLabel1MousePressed(java.awt.event.MouseEvent evt) {
        jLabel1
                .setIcon(new ImageIcon(cl.getResource("gui/resources/1_on.gif")));
    }

    public void jLabel1MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel1.setIcon(new ImageIcon(cl.getResource("gui/resources/1.gif")));
    }

    public void jLabel1MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "1");
    }

    public void jLabel2MousePressed(java.awt.event.MouseEvent evt) {
        jLabel2
                .setIcon(new ImageIcon(cl.getResource("gui/resources/2_on.gif")));
    }

    public void jLabel2MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel2.setIcon(new ImageIcon(cl.getResource("gui/resources/2.gif")));
    }

    public void jLabel2MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "2");
    }

    public void jLabel3MousePressed(java.awt.event.MouseEvent evt) {
        jLabel3
                .setIcon(new ImageIcon(cl.getResource("gui/resources/3_on.gif")));
    }

    public void jLabel3MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel3.setIcon(new ImageIcon(cl.getResource("gui/resources/3.gif")));
    }

    public void jLabel3MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "3");
    }

    public void jLabel4MousePressed(java.awt.event.MouseEvent evt) {
        jLabel4
                .setIcon(new ImageIcon(cl.getResource("gui/resources/4_on.gif")));
    }

    public void jLabel4MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel4.setIcon(new ImageIcon(cl.getResource("gui/resources/4.gif")));
    }

    public void jLabel4MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "4");
    }

    public void jLabel5MousePressed(java.awt.event.MouseEvent evt) {
        jLabel5
                .setIcon(new ImageIcon(cl.getResource("gui/resources/5_on.gif")));
    }

    public void jLabel5MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel5.setIcon(new ImageIcon(cl.getResource("gui/resources/5.gif")));
    }

    public void jLabel5MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "5");
    }

    public void jLabel6MousePressed(java.awt.event.MouseEvent evt) {
        jLabel6
                .setIcon(new ImageIcon(cl.getResource("gui/resources/6_on.gif")));
    }

    public void jLabel6MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel6.setIcon(new ImageIcon(cl.getResource("gui/resources/6.gif")));
    }

    public void jLabel6MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "6");
    }

    public void jLabel7MousePressed(java.awt.event.MouseEvent evt) {
        jLabel7
                .setIcon(new ImageIcon(cl.getResource("gui/resources/7_on.gif")));
    }

    public void jLabel7MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel7.setIcon(new ImageIcon(cl.getResource("gui/resources/7.gif")));
    }

    public void jLabel7MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "7");
    }

    public void jLabel8MousePressed(java.awt.event.MouseEvent evt) {
        jLabel8
                .setIcon(new ImageIcon(cl.getResource("gui/resources/8_on.gif")));
    }

    public void jLabel8MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel8.setIcon(new ImageIcon(cl.getResource("gui/resources/8.gif")));
    }

    public void jLabel8MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "8");
    }

    public void jLabel9MousePressed(java.awt.event.MouseEvent evt) {
        jLabel9
                .setIcon(new ImageIcon(cl.getResource("gui/resources/9_on.gif")));
    }

    public void jLabel9MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel9.setIcon(new ImageIcon(cl.getResource("gui/resources/9.gif")));
    }

    public void jLabel9MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "9");
    }

    public void jLabel10MousePressed(java.awt.event.MouseEvent evt) {
        jLabel10.setIcon(new ImageIcon(cl
                .getResource("gui/resources/asterisk_on.gif")));
    }

    public void jLabel10MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel10.setIcon(new ImageIcon(cl
                .getResource("gui/resources/asterisk.gif")));
    }

    public void jLabel11MousePressed(java.awt.event.MouseEvent evt) {
        jLabel11
                .setIcon(new ImageIcon(cl.getResource("gui/resources/0_on.gif")));
    }

    public void jLabel11MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel11.setIcon(new ImageIcon(cl.getResource("gui/resources/0.gif")));
    }

    public void jLabel11MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "0");
    }

    public void jLabel12MousePressed(java.awt.event.MouseEvent evt) {
        jLabel12.setIcon(new ImageIcon(cl
                .getResource("gui/resources/sharp_on.gif")));
    }

    public void jLabel12MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel12.setIcon(new ImageIcon(cl
                .getResource("gui/resources/sharp.gif")));
    }

    public void jLabel13MousePressed(java.awt.event.MouseEvent evt) {
        jLabel13.setIcon(new ImageIcon(cl
                .getResource("gui/resources/phone_up_on.gif")));
    }

    private void jLabel13MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel13.setIcon(new ImageIcon(cl
                .getResource("gui/resources/phone_up.gif")));

        String sipurl = jTextField1.getText();

        if (!sipurl.trim().equalsIgnoreCase("")) {
            if (actacc != null) {
                if (actacc.registered) {

                    Vector v = new Vector();
                    v.add(new Integer(actacc.id));
                    v.add(sipurl);
                    Object o = execute("core.makeCall", v);
                    if (o != null) {
                        int id = ((Integer) o).intValue();
                        Call c = new Call(id, actacc.id, sipurl, dname);
                        dname = "";
                        calls.add(c);
                        ImageIcon img = new ImageIcon(cl
                                .getResource("gui/resources/gray.gif"));
                        if (c.name.trim().equalsIgnoreCase("")) {
                            img.setDescription(sipurl);
                        } else {
                            img.setDescription(sipurl + " (" + c.name + ")");
                        }
                        list2.addElement(img);
                        print("Trying to call " + sipurl + " (call # " + id
                                + ").");
                    }
                } else {
                    JOptionPane.showMessageDialog(this, "Account #" + actacc.id
                            + " not registered.", "ERROR",
                            JOptionPane.ERROR_MESSAGE);
                }
            } else {
                JOptionPane.showMessageDialog(this, "No account selected.",
                        "ERROR", JOptionPane.ERROR_MESSAGE);
            }
        } else {
            JOptionPane.showMessageDialog(this, "Enter a SIP URL to call.",
                    "ERROR", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void jLabel14MousePressed(java.awt.event.MouseEvent evt) {
        if (jList2.getSelectedIndex() > -1) {
            jLabel14.setIcon(new ImageIcon(cl
                    .getResource("gui/resources/phone_down_on.gif")));
        }
    }

    private void jLabel14MouseReleased(java.awt.event.MouseEvent evt) {
        int i = jList2.getSelectedIndex();
        if (i > -1) {
            Call c = (Call) calls.elementAt(i);
            params.clear();
            params.add(new Integer(c.id));
            execute("core.endCall", params);
            list2.removeElementAt(i);
            ImageIcon img = new ImageIcon(cl
                    .getResource("gui/resources/red.gif"));
            img.setDescription(c.sipurl);
            list2.insertElementAt(img, i);
            print("Terminating call # " + c.id + " ...");
            jLabel14.setIcon(new ImageIcon(cl
                    .getResource("gui/resources/phone_down.gif")));
        }
    }

    public void jLabel15MousePressed(java.awt.event.MouseEvent evt) {
        jLabel15.setIcon(new ImageIcon(cl
                .getResource("gui/resources/addressbook_on.gif")));
    }

    public void jLabel15MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel15.setIcon(new ImageIcon(cl
                .getResource("gui/resources/addressbook.gif")));
        addressBook.setVisible(true);
    }

    /* "Register" button clicked */
    public void jButton1MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList1.getSelectedIndex();
        if (i > -1) {

            Account acc = (Account) accounts.elementAt(i);
            Integer accId = new Integer(acc.id);

            if (acc.registered) {
                params.clear();
                params.add(accId);
                list1.set(i, "#" + accId.intValue() + ": unregistering ...");
                execute("core.unregister", params);
            } else {
                params.clear();
                params.add(accId);
                list1.set(i, "#" + accId.intValue() + ": registering ...");
                execute("core.register", params);
            }

        }
    }

    /* "Unregister" button clicked */
    public void jButton2MouseClicked(java.awt.event.MouseEvent evt) {
        execute("core.accountSave", params);
    }

    /* "Set Values" button clicked */
    public void jButton3MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList1.getSelectedIndex();
        if (i > -1) {
            Integer accId = new Integer(((Account) accounts.elementAt(i)).id);
            setValues(accId);
        }
    }

    private void setValues(Integer accId) {
        // set name
        params.clear();
        params.add(accId);
        params.add("name");
        params.add(jTextField8.getText());
        execute("core.accountSet", params);
        // set displayname
        params.clear();
        params.add(accId);
        params.add("displayname");
        params.add(jTextField2.getText());
        execute("core.accountSet", params);
        // set username
        params.clear();
        params.add(accId);
        params.add("username");
        params.add(jTextField3.getText());
        execute("core.accountSet", params);
        // set authusername
        params.clear();
        params.add(accId);
        params.add("authusername");
        params.add(jTextField4.getText());
        execute("core.accountSet", params);
        // set domain
        params.clear();
        params.add(accId);
        params.add("domain");
        params.add(jTextField5.getText());
        execute("core.accountSet", params);
        // set outboundproxy
        params.clear();
        params.add(accId);
        params.add("outboundproxy");
        params.add(jTextField6.getText());
        execute("core.accountSet", params);
        // set registrar
        params.clear();
        params.add(accId);
        params.add("registrar");
        params.add(jTextField7.getText());
        execute("core.accountSet", params);
        // set password
        params.clear();
        params.add(accId);
        params.add("password");
        params.add(new String(jPasswordField1.getPassword()));
        execute("core.accountSet", params);
        // set autoregister
        params.clear();
        params.add(accId);
        params.add("autoregister");
        if (jCheckBox1.isSelected()) {
            params.add("1");
        } else {
            params.add("0");
        }
        execute("core.accountSet", params);
    }

    /* "Delete" button clicked */
    public void jButton4MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList1.getSelectedIndex();
        if (i > -1) {
            Integer accId = new Integer(((Account) accounts.elementAt(i)).id);

            params.clear();
            params.add(accId);
            if (((Boolean) execute("core.accountDelete", params))
                    .booleanValue()) {
                list1.remove(i);
                accounts.remove(i);
                jTextField2.setText("");
                jTextField3.setText("");
                jTextField4.setText("");
                jTextField5.setText("");
                jTextField6.setText("");
                jTextField7.setText("");
                jTextField8.setText("");
                jPasswordField1.setText("");
                jCheckBox1.setSelected(false);
                jTextArea1.append(Utils.getTimestamp() + ": Account with ID "
                        + accId.intValue() + " removed.\n");
            }
        }
    }

    /* "Create" button clicked */
    public void jButton5MouseClicked(java.awt.event.MouseEvent evt) {
        params.clear();
        Integer accId = (Integer) execute("core.accountCreate", params);
        if (accId != null) {
            Account acc = new Account(accId.intValue(), false);
            accounts.add(acc);
            list1.addElement("#" + accId.toString() + ": not registered");
            setValues(accId);
        }
    }

    /* "Clear" button clicked */
    public void jButton6MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField2.setText("");
        jTextField3.setText("");
        jTextField4.setText("");
        jTextField5.setText("");
        jTextField6.setText("");
        jTextField7.setText("");
        jTextField8.setText("");
        jPasswordField1.setText("");
        jCheckBox1.setSelected(false);
    }

    /* Phonebook "Select" button clicked */
    public void jButton7MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList3.getSelectedIndex();
        if (i > -1) {
            Contact c = (Contact) phonebook.elementAt(i);
            jTextField1.setText(c.sipurl);
            dname = c.name;
            addressBook.setVisible(false);
        }
    }

    /* Phonebook "Create" button clicked */
    public void jButton8MouseClicked(java.awt.event.MouseEvent evt) {
        String name = jTextField9.getText();
        String phone = jTextField10.getText();
        if (name.equalsIgnoreCase("") || phone.equalsIgnoreCase("")) {
            JOptionPane.showMessageDialog(addressBook,
                    "Please fill both fields!", "ERROR",
                    JOptionPane.ERROR_MESSAGE);
        } else {
            Contact c = new Contact(name, phone);
            phonebook.addElement(c);
            list3.addElement(name);
            jTextField9.setText("");
            jTextField10.setText("");

            savePhonebook();
        }
    }

    /* Phonebook "Delete" button clicked */
    public void jButton9MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList3.getSelectedIndex();
        if (i > -1) {
            phonebook.remove(i);
            list3.removeElementAt(i);
            jTextField9.setText("");
            jTextField10.setText("");

            savePhonebook();
        }
    }

    public void jSlider1StateChanged(javax.swing.event.ChangeEvent evt) {
        jLabel16.setText(jSlider1.getValue() + "%");
    }

    public void jSlider1MousePressed(java.awt.event.MouseEvent evt) {
        slidervalue = jSlider1.getValue();
    }

    public void jSlider1MouseReleased(java.awt.event.MouseEvent evt) {
        int v = jSlider1.getValue();
        if (v != slidervalue) {
            params.clear();
            params.add(new Double((double) v / 100));
            execute("core.setSpeakerVolume", params);
        }
    }

    public void jSlider2StateChanged(javax.swing.event.ChangeEvent evt) {
        jLabel26.setText(jSlider2.getValue() + "%");
    }

    public void jSlider2MousePressed(java.awt.event.MouseEvent evt) {
        slidervalue = jSlider2.getValue();
    }

    public void jSlider2MouseReleased(java.awt.event.MouseEvent evt) {
        int v = jSlider2.getValue();
        if (v != slidervalue) {
            params.clear();
            params.add(new Double((double) v / 100));
            execute("core.setMicroVolume", params);
        }
    }

    private void jList1ValueChanged(javax.swing.event.ListSelectionEvent evt) {
        int i = jList1.getSelectedIndex();
        if (i > -1) {

            Account acc = (Account) accounts.elementAt(i);
            actacc = acc;
            Integer accId = new Integer(acc.id);

            if (acc.registered) {
                jButton1.setText("Unregister");
            } else {
                jButton1.setText("Register");
            }

            jButton1.setEnabled(true);
            jButton2.setEnabled(true);
            jButton3.setEnabled(true);
            jButton4.setEnabled(true);

            params.clear();
            params.add(accId);
            params.add("name");
            jTextField8.setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("displayname");
            jTextField2.setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("username");
            jTextField3.setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("authusername");
            jTextField4.setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("domain");
            jTextField5.setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("outboundproxy");
            jTextField6.setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("registrar");
            jTextField7.setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("password");
            jPasswordField1
                    .setText((String) execute("core.accountGet", params));
            params.clear();
            params.add(accId);
            params.add("autoregister");
            String s = (String) execute("core.accountGet", params);
            if (s.equalsIgnoreCase("1")) {
                jCheckBox1.setSelected(true);
            } else {
                jCheckBox1.setSelected(false);
            }
        } else {
            jButton1.setEnabled(false);
            jButton3.setEnabled(false);
            jButton4.setEnabled(false);
            actacc = null;
        }
    }

    private void jList2ValueChanged(javax.swing.event.ListSelectionEvent evt) {
        int i = jList2.getSelectedIndex();
        if (i > -1) {
            jLabel14.setIcon(new ImageIcon(cl
                    .getResource("gui/resources/phone_down.gif")));
        } else {
            jLabel14.setIcon(new ImageIcon(cl
                    .getResource("gui/resources/phone_down_dis.gif")));
        }
    }

    private void jList3ValueChanged(javax.swing.event.ListSelectionEvent evt) {
        int i = jList3.getSelectedIndex();
        if (i > -1) {
            Contact c = (Contact) phonebook.elementAt(i);
            jTextField9.setText(c.name);
            jTextField10.setText(c.sipurl);
        }
    }

    private void print(String s) {
        jTextArea1.append(Utils.getTimestamp() + ": " + s + "\n");
    }

    /**
     * Execute a XML-RPC call with specified timeout.
     *
     * @param s
     * @param v
     * @param p
     * @return
     */
    public Object execute(String s, Vector v, int p) {
        ExeThread t = new ExeThread(client, s, v);
        t.setTimeout(p);
        return t.execute();
    }

    /**
     * Execute a XML-RPC call with standard timeout.
     *
     * @param s
     * @param v
     * @return
     */
    public final Object execute(String s, Vector v) {
        ExeThread t = new ExeThread(client, s, v);
        return t.execute();
    }

    private final void readPhonebook() {
        try {

            SAXParser parser = new SAXParser();

            PhonebookParser pp = new PhonebookParser(phonebook);

            parser.setContentHandler(pp);

            parser.parse(accfile);

        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (SAXException e) {
            System.out.println("> Your XML file seems to be invalid.");
        } catch (IOException e) {
            System.out.println("> No phonebook found.");
        }
    }

    private final void savePhonebook() {
        try {
            FileOutputStream fis = new FileOutputStream(accfile);
            OutputStreamWriter out = new OutputStreamWriter(fis, "UTF8");
            BufferedWriter bw = new BufferedWriter(out);

            bw.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            bw.write("<phonebook>\n");

            Enumeration e = phonebook.elements();
            while (e.hasMoreElements()) {
                Contact con = (Contact) e.nextElement();
                bw.write("\t<entry name=\"" + con.name + "\" number=\""
                        + con.sipurl + "\"/>\n");
            }

            bw.write("</phonebook>\n");
            bw.flush();
            bw.close();
            out.close();
            fis.close();

        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }
}

class PhonebookParser extends DefaultHandler {

    private Vector phonebook;

    public PhonebookParser(Vector p) {
        phonebook = p;
    }

    public void startElement(String uri, String local, String qName,
            Attributes atts) {
        if (local.equalsIgnoreCase("entry")) {

            String name = "";
            String sip = "";

            for (int i = 0; i < atts.getLength(); i++) {
                String attname = atts.getLocalName(i);
                if (attname.equalsIgnoreCase("name")) {
                    name = atts.getValue(attname);
                } else if (attname.equalsIgnoreCase("number")) {
                    sip = atts.getValue(attname);
                }
            }
            Contact c = new Contact(name, sip);
            phonebook.add(c);
        }
    }
}