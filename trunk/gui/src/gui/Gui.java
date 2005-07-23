package gui;

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
import org.apache.xmlrpc.WebServer;
import org.apache.xmlrpc.XmlRpcClientLite;
import org.netbeans.lib.awtextra.AbsoluteConstraints;
import org.netbeans.lib.awtextra.AbsoluteLayout;
import java.util.Enumeration;

/**
 *
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */
public class Gui extends JFrame {
    
    boolean phone_up_active = true;
    public boolean phone_down_active = false;
    
    private String GUI_HOST = "192.168.0.1";

    private String CORE_HOST = "192.168.0.1";

    private int GUI_PORT = 8888;

    private int CORE_PORT = 7777;

    Vector accountId = new Vector();

    Vector callId = new Vector();

    //just for testing

    // client part - sends calls to core
    XmlRpcClientLite client;

    // server part - processes calls from core
    WebServer server;

    // stores the parameters for a call
    Vector params = new Vector();

    
    /** Creates new form NewApplication */
    public Gui() {
        initComponents();
        
        try {

            client = new XmlRpcClientLite(InetAddress.getByName(CORE_HOST)
                    .getHostName(), CORE_PORT);

            server = new WebServer(GUI_PORT, InetAddress.getByName(GUI_HOST));

            GuiStub guiStub = new GuiStub(this);

            server.start();
            server.addHandler("gui", guiStub);
            
            // call REGISTER_GUI immediately after GUI has been opened
            params.clear();
            params.addElement(GUI_HOST);
            params.addElement(new Integer(GUI_PORT));     
                
                Object o = execute("core.registerGui", params, 3000);
                if (o == null) {
// --- something
                } else if (((String) o).equalsIgnoreCase("OK")) {
                    jTextArea1.append(Utils.getTimestamp()+": GUI registration successful.\n");
                    
                    params.clear();
                    o = execute("core.accountGetAll", params);
                    
                    if (o != null && o instanceof Vector) {
                        Vector v = (Vector) o;
                        Enumeration e = v.elements();
                        while (e.hasMoreElements()) {
                            Integer n = (Integer) e.nextElement();
                            list1.addElement(n.toString());
                            accounts.add(n);
                        }
                    }
                    
                } else {
                    // ERROR returned
                }

        } catch (MalformedURLException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        } catch (UnknownHostException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }

        
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">
    private void initComponents() {
        jTabbedPane1 = new JTabbedPane();
        jPanel1 = new JPanel();
        //jComboBox1 = new JComboBox();
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
        jSlider1 = new JSlider();
        jSlider2 = new JSlider();
        menuBar = new JMenuBar();
        helpMenu = new JMenu();
        contentsMenuItem = new JMenuItem();
        aboutMenuItem = new JMenuItem();
        jMenuItem1 = new JMenuItem();
        jMenuItem2 = new JMenuItem();
        jScrollPane1 = new JScrollPane();
        jScrollPane2 = new JScrollPane();
        jScrollPane3 = new JScrollPane();
        list1 = new DefaultListModel();
        list2 = new DefaultListModel();
        jList1 = new JList(list1);
        jList2 = new JList(list2);
        jLabel17 = new JLabel();
        jLabel18 = new JLabel();
        jLabel19 = new JLabel();
        jLabel20 = new JLabel();
        jLabel21 = new JLabel();
        jLabel22 = new JLabel();
        jTextField4 = new JTextField();
        jLabel23 = new JLabel();
        jLabel24 = new JLabel();
        jTextField2 = new JTextField();
        jTextField3 = new JTextField();
        jTextField5 = new JTextField();
        jPasswordField1 = new JPasswordField();
        jTextField6 = new JTextField();
        jTextField7 = new JTextField();
        jTextField8 = new JTextField();
        jLabel25 = new JLabel();
        jCheckBox1 = new JCheckBox();
        jButton1 = new JButton();
        jButton2 = new JButton();
        jButton3 = new JButton();
        jButton4 = new JButton();
        jButton5 = new JButton();
        jTextArea1 = new JTextArea();
        accounts = new Vector();
        
        getContentPane().setLayout(new AbsoluteLayout());

        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        jPanel1.setLayout(new AbsoluteLayout());


        jPanel1.add(jTextField1, new AbsoluteConstraints(170, 10, 410, -1));

        cl = this.getClass().getClassLoader();
        
        jLabel1.setIcon(new ImageIcon(cl.getResource("test/ressources/1.gif")));
        jPanel1.add(jLabel1, new AbsoluteConstraints(10, 10, -1, -1));

        jLabel2.setIcon(new ImageIcon(cl.getResource("test/ressources/2.gif")));
        jPanel1.add(jLabel2, new AbsoluteConstraints(60, 10, -1, -1));

        jLabel3.setIcon(new ImageIcon(cl.getResource("test/ressources/3.gif")));
        jPanel1.add(jLabel3, new AbsoluteConstraints(110, 10, -1, -1));

        jLabel4.setIcon(new ImageIcon(cl.getResource("test/ressources/4.gif")));
        jPanel1.add(jLabel4, new AbsoluteConstraints(10, 60, -1, -1));

        jLabel5.setIcon(new ImageIcon(cl.getResource("test/ressources/5.gif")));
        jPanel1.add(jLabel5, new AbsoluteConstraints(60, 60, -1, -1));

        jLabel6.setIcon(new ImageIcon(cl.getResource("test/ressources/6.gif")));
        jPanel1.add(jLabel6, new AbsoluteConstraints(110, 60, -1, -1));

        jLabel7.setIcon(new ImageIcon(cl.getResource("test/ressources/7.gif")));
        jPanel1.add(jLabel7, new AbsoluteConstraints(10, 110, -1, -1));

        jLabel8.setIcon(new ImageIcon(cl.getResource("test/ressources/8.gif")));
        jPanel1.add(jLabel8, new AbsoluteConstraints(60, 110, -1, -1));

        jLabel9.setIcon(new ImageIcon(cl.getResource("test/ressources/9.gif")));
        jPanel1.add(jLabel9, new AbsoluteConstraints(110, 110, -1, -1));

        jLabel10.setIcon(new ImageIcon(cl.getResource("test/ressources/asterisk.gif")));
        jPanel1.add(jLabel10, new AbsoluteConstraints(10, 160, -1, -1));

        jLabel11.setIcon(new ImageIcon(cl.getResource("test/ressources/0.gif")));
        jPanel1.add(jLabel11, new AbsoluteConstraints(60, 160, -1, -1));

        jLabel12.setIcon(new ImageIcon(cl.getResource("test/ressources/sharp.gif")));
        jPanel1.add(jLabel12, new AbsoluteConstraints(110, 160, -1, -1));

        jLabel13.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_up.gif")));
        jLabel13.setVerticalAlignment(SwingConstants.TOP);
        jLabel13.setPreferredSize(new java.awt.Dimension(54, 54));
        jPanel1.add(jLabel13, new AbsoluteConstraints(590, 10, 54, 54));
        
        jLabel14.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_down_dis.gif")));
        jLabel14.setVerticalAlignment(SwingConstants.TOP);
        jLabel14.setPreferredSize(new java.awt.Dimension(54, 54));
        jPanel1.add(jLabel14, new AbsoluteConstraints(590, 150, 54, 54));

        jLabel15.setIcon(new ImageIcon(cl.getResource("test/ressources/addressbook.gif")));
        jLabel15.setVerticalAlignment(SwingConstants.TOP);
        jLabel15.setPreferredSize(new java.awt.Dimension(54, 54));
        jPanel1.add(jLabel15, new AbsoluteConstraints(590, 80, 54, 54));

        
        jScrollPane2.setAutoscrolls(true);
        jTextArea1.setBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)));
        jScrollPane2.setViewportView(jTextArea1);
        
        jPanel1.add(jScrollPane2, new AbsoluteConstraints(10, 220, 640, 120));
        
        jList2.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        jScrollPane3.setAutoscrolls(true);
        jList2.setBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)));
        jList2.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jList2.setValueIsAdjusting(true);
        jScrollPane3.setViewportView(jList2);
        
        jPanel1.add(jScrollPane3, new AbsoluteConstraints(170, 40, 410, 170));
        
        jTabbedPane1.addTab("Calls", jPanel1);

        jPanel3.setLayout(new AbsoluteLayout());

        jTabbedPane1.addTab("Accounts", jPanel3);
        jList1.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        jScrollPane1.setAutoscrolls(true);
        jList1.setBorder(new javax.swing.border.EmptyBorder(new java.awt.Insets(1, 1, 1, 1)));
        jList1.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
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
        jPanel3.add(jPasswordField1, new AbsoluteConstraints(350, 170, 200, -1));

        jPanel3.add(jTextField6, new AbsoluteConstraints(350, 200, 200, -1));

        jPanel3.add(jTextField7, new AbsoluteConstraints(350, 230, 200, -1));

        jLabel25.setText("Auto-Register:");
        jPanel3.add(jLabel25, new AbsoluteConstraints(210, 260, -1, -1));

        jPanel3.add(jCheckBox1, new AbsoluteConstraints(350, 260, -1, -1));
        
        jButton1.setText("Register");
        jPanel3.add(jButton1, new AbsoluteConstraints(270, 300, 130, -1));

        jButton2.setText("Unregister");
        jPanel3.add(jButton2, new AbsoluteConstraints(400, 300, 130, -1));

        jButton3.setText("Set Values");
        jPanel3.add(jButton3, new AbsoluteConstraints(530, 300, 130, -1));
        
        jButton4.setText("Delete Account");
        jPanel3.add(jButton4, new AbsoluteConstraints(140, 300, 130, -1));

        jButton5.setText("Create Account");
        jPanel3.add(jButton5, new AbsoluteConstraints(10, 300, 130, -1));

        jPanel3.add(jScrollPane1, new AbsoluteConstraints(10, 10, 190, 260));

        jPanel4.setLayout(new AbsoluteLayout());

        jSlider1.setMinimum(0);
        jSlider1.setMaximum(105);
        jSlider1.setExtent(5);
        jSlider1.setValue(50);
        
        jSlider2.setMinimum(0);
        jSlider2.setMaximum(105);
        jSlider2.setExtent(5);
        jSlider2.setValue(50);
        
        jLabel16.setText("Level: ");
        jPanel4.add(jLabel16, new AbsoluteConstraints(210, 20, -1, -1));
        
        jPanel4.add(jSlider1, new AbsoluteConstraints(40, 20, 160, -1));

        jPanel4.add(jSlider2, new AbsoluteConstraints(40, 50, 160, -1));
        
        jTabbedPane1.addTab("Options", jPanel4);

        getContentPane().add(jTabbedPane1, new AbsoluteConstraints(0, 0, 670, 380));

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
        
        jList1.addListSelectionListener(new javax.swing.event.ListSelectionListener() {
            public void valueChanged(javax.swing.event.ListSelectionEvent evt) {
                jList1ValueChanged(evt);
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
        
        jSlider1.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSlider1StateChanged(evt);
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
     * @param args the command line arguments
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
    //public JComboBox jComboBox1;
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
    public JSlider jSlider1;
    public JSlider jSlider2;
    public JTabbedPane jTabbedPane1;
    public JMenuBar menuBar;
    public JList jList1;
    public JList jList2;
    public ClassLoader cl;
    public DefaultListModel list1;
    public DefaultListModel list2;
    public JScrollPane jScrollPane1;
    public JScrollPane jScrollPane2;
    public JScrollPane jScrollPane3;
    public JTextField jTextField2;
    public JTextField jTextField3;
    public JTextField jTextField4;
    public JTextField jTextField5;
    public JTextField jTextField6;
    public JTextField jTextField7;
    public JTextField jTextField8;
    public JPasswordField jPasswordField1;
    public JButton jButton1;
    public JButton jButton2;
    public JButton jButton3;
    public JButton jButton4;
    public JButton jButton5;
    public JCheckBox jCheckBox1;
    public JTextArea jTextArea1;
    public Vector accounts;
    // End of variables declaration
    
    public void jLabel1MousePressed(java.awt.event.MouseEvent evt) {
        jLabel1.setIcon(new ImageIcon(cl.getResource("test/ressources/1_on.gif")));
         }   
    public void jLabel1MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel1.setIcon(new ImageIcon(cl.getResource("test/ressources/1.gif")));
         }
    public void jLabel1MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "1");
         }
    
    public void jLabel2MousePressed(java.awt.event.MouseEvent evt) {
        jLabel2.setIcon(new ImageIcon(cl.getResource("test/ressources/2_on.gif")));
         }   
    public void jLabel2MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel2.setIcon(new ImageIcon(cl.getResource("test/ressources/2.gif")));
         }
    public void jLabel2MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "2");
         }
    
    public void jLabel3MousePressed(java.awt.event.MouseEvent evt) {
        jLabel3.setIcon(new ImageIcon(cl.getResource("test/ressources/3_on.gif")));
         }   
    public void jLabel3MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel3.setIcon(new ImageIcon(cl.getResource("test/ressources/3.gif")));
         }
    public void jLabel3MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "3");
         }
    
    public void jLabel4MousePressed(java.awt.event.MouseEvent evt) {
        jLabel4.setIcon(new ImageIcon(cl.getResource("test/ressources/4_on.gif")));
         }   
    public void jLabel4MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel4.setIcon(new ImageIcon(cl.getResource("test/ressources/4.gif")));
         }
    public void jLabel4MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "4");
         }
    
    public void jLabel5MousePressed(java.awt.event.MouseEvent evt) {
        jLabel5.setIcon(new ImageIcon(cl.getResource("test/ressources/5_on.gif")));
         }   
    public void jLabel5MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel5.setIcon(new ImageIcon(cl.getResource("test/ressources/5.gif")));
         }
    public void jLabel5MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "5");
         }
    
    public void jLabel6MousePressed(java.awt.event.MouseEvent evt) {
        jLabel6.setIcon(new ImageIcon(cl.getResource("test/ressources/6_on.gif")));
         }   
    public void jLabel6MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel6.setIcon(new ImageIcon(cl.getResource("test/ressources/6.gif")));
         }
    public void jLabel6MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "6");
         }
    
    public void jLabel7MousePressed(java.awt.event.MouseEvent evt) {
        jLabel7.setIcon(new ImageIcon(cl.getResource("test/ressources/7_on.gif")));
         }   
    public void jLabel7MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel7.setIcon(new ImageIcon(cl.getResource("test/ressources/7.gif")));
         }
    public void jLabel7MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "7");
         }
    
    public void jLabel8MousePressed(java.awt.event.MouseEvent evt) {
        jLabel8.setIcon(new ImageIcon(cl.getResource("test/ressources/8_on.gif")));
         }   
    public void jLabel8MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel8.setIcon(new ImageIcon(cl.getResource("test/ressources/8.gif")));
         }
    public void jLabel8MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "8");
         }
    
    public void jLabel9MousePressed(java.awt.event.MouseEvent evt) {
        jLabel9.setIcon(new ImageIcon(cl.getResource("test/ressources/9_on.gif")));
         }   
    public void jLabel9MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel9.setIcon(new ImageIcon(cl.getResource("test/ressources/9.gif")));
         }
    public void jLabel9MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "9");
         }
    
    public void jLabel10MousePressed(java.awt.event.MouseEvent evt) {
        jLabel10.setIcon(new ImageIcon(cl.getResource("test/ressources/asterisk_on.gif")));
         }   
    public void jLabel10MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel10.setIcon(new ImageIcon(cl.getResource("test/ressources/asterisk.gif")));
         }
    
    public void jLabel11MousePressed(java.awt.event.MouseEvent evt) {
        jLabel11.setIcon(new ImageIcon(cl.getResource("test/ressources/0_on.gif")));
         }   
    public void jLabel11MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel11.setIcon(new ImageIcon(cl.getResource("test/ressources/0.gif")));
         }
    public void jLabel11MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField1.setText(jTextField1.getText() + "0");
         }
    
    public void jLabel12MousePressed(java.awt.event.MouseEvent evt) {
        jLabel12.setIcon(new ImageIcon(cl.getResource("test/ressources/sharp_on.gif")));
         }   
    public void jLabel12MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel12.setIcon(new ImageIcon(cl.getResource("test/ressources/sharp.gif")));
         }
    
    public void jLabel13MousePressed(java.awt.event.MouseEvent evt) {
        jLabel13.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_up_on.gif")));
         }   
    private void jLabel13MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel13.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_up.gif")));
        
        Vector v = new Vector();
        v.add(new Integer(1));
        v.add(new String("Bob"));
        //ExeThread t = new ExeThread(client);
        Object o = execute("core.makeCall", v);
    }
    
    private void jLabel14MousePressed(java.awt.event.MouseEvent evt) {
        if (phone_down_active)
        jLabel14.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_down_on.gif")));
         }   
    private void jLabel14MouseReleased(java.awt.event.MouseEvent evt) {
        if (phone_down_active)
            jLabel14.setIcon(new ImageIcon(cl.getResource("test/ressources/phone_down_dis.gif")));
        	//jLabel14.setEnabled(false);
        	phone_down_active = false;
        	int i = jList2.getSelectedIndex();
        	jTextArea1.append(Utils.getTimestamp()+": Connection to " + (String) jList2.getSelectedValue()+" terminated.\n");
        	list2.remove(i);
         }
    
    public void jLabel15MousePressed(java.awt.event.MouseEvent evt) {
        jLabel15.setIcon(new ImageIcon(cl.getResource("test/ressources/addressbook_on.gif")));
         }   
    public void jLabel15MouseReleased(java.awt.event.MouseEvent evt) {
        jLabel15.setIcon(new ImageIcon(cl.getResource("test/ressources/addressbook.gif")));
         }
    
    /* "Register" button clicked */
    public void jButton1MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList1.getSelectedIndex();
        Integer accId = (Integer) accounts.elementAt(i);   
        
        params.clear();
        params.add(accounts.elementAt(i));    
        if (((Boolean) execute("core.register", params)).booleanValue()) {
            list1.set(i, accounts.elementAt(i)+": registered");
            jTextArea1.append(Utils.getTimestamp()+": Account with ID "+ accId.toString()+" registered.\n");
        }
    }
    
    /* "Unregister" button clicked */
    public void jButton2MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList1.getSelectedIndex();
        Integer accId = (Integer) accounts.elementAt(i);
        
        params.clear();
        params.add(accounts.elementAt(i));    
        if (((Boolean) execute("core.unregister", params)).booleanValue()) {
            list1.set(i, accounts.elementAt(i).toString());
            jTextArea1.append(Utils.getTimestamp()+": Account with ID "+  accId+   " unregistered.\n");
        }
    }
    
    /* "Set Values" button clicked */
    public void jButton3MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList1.getSelectedIndex();
        //Integer accId = new Integer(i+1);
        Integer accId = (Integer) accounts.elementAt(i);    
        
        // set name
        params.clear();
        params.add(accId);
        params.add(jTextField8.getText());
        execute("core.accountSet", params);
        // set displayname
        params.clear();
        params.add(accId);
        params.add(jTextField2.getText());
        execute("core.accountSet", params);
        // set username
        params.clear();
        params.add(accId);
        params.add(jTextField3.getText());
        execute("core.accountSet", params);
        // set authusername
        params.clear();
        params.add(accId);
        params.add(jTextField4.getText());
        execute("core.accountSet", params);
        // set domain
        params.clear();
        params.add(accId);
        params.add(jTextField5.getText());
        execute("core.accountSet", params);
        // set outboundproxy
        params.clear();
        params.add(accId);
        params.add(jTextField6.getText());
        execute("core.accountSet", params);
        // set registrar
        params.clear();
        params.add(accId);
        params.add(jTextField7.getText());
        execute("core.accountSet", params);
        // set password
        params.clear();
        params.add(accId);
        params.add(new String(jPasswordField1.getPassword()));
        execute("core.accountSet", params);
    }
    
    /* "Delete" button clicked */
    public void jButton4MouseClicked(java.awt.event.MouseEvent evt) {
        int i = jList1.getSelectedIndex();
        Integer accId = (Integer) accounts.elementAt(i);
        
        params.clear();
        params.add(accounts.elementAt(i));        
        if (((Boolean) execute("core.accountDelete", params)).booleanValue()) {
            list1.remove(i);
            accounts.remove(i);
            jTextArea1.append(Utils.getTimestamp()+": Account with ID "+accId+" removed.\n");
        }
    }
    
    /* "Create" button clicked */
    public void jButton5MouseClicked(java.awt.event.MouseEvent evt) {
        jTextField3.setText("5");
        //jLabel15.setIcon(new ImageIcon(cl.getResource("test/ressources/addressbook.gif")));
    }
    
    public void jSlider1StateChanged(javax.swing.event.ChangeEvent evt) {
        jLabel16.setText("Level: "+jSlider1.getValue()+"%");
         }    
    
    private void jList1ValueChanged(javax.swing.event.ListSelectionEvent evt) {
        int i = jList1.getSelectedIndex();
        //Integer accId = (Integer) accounts.elementAt(i);    
        Integer accId = new Integer(i+1);
        
        params.clear();
        params.add(accId);
        params.add(new String("name"));
        jTextField8.setText((String) execute("core.accountGet",params));
        params.clear();
        params.add(accId);
        params.add(new String("displayname"));
        jTextField2.setText((String) execute("core.accountGet",params));
        params.clear();
        params.add(accId);
        params.add(new String("username"));
        jTextField3.setText((String) execute("core.accountGet",params));
        params.clear();
        params.add(accId);
        params.add(new String("authusername"));
        jTextField4.setText((String) execute("core.accountGet",params));
        params.clear();
        params.add(accId);
        params.add(new String("domain"));
        jTextField5.setText((String) execute("core.accountGet",params));
        params.clear();
        params.add(accId);
        params.add(new String("outboundproxy"));
        jTextField6.setText((String) execute("core.accountGet",params));
        params.clear();
        params.add(accId);
        params.add(new String("registrar"));
        jTextField7.setText((String) execute("core.accountGet",params));
        params.clear();
        params.add(accId);
        params.add(new String("password"));
        jPasswordField1.setText((String) execute("core.accountGet",params));
    }
    
    /**
     * Execute with specified timeout.
     * 
     * @param s
     * @param v
     * @param p
     * @return
     */
    private Object execute(String s, Vector v, int p) {
        ExeThread t = new ExeThread(client, s, v);
        /* if (p > 0) {
            t.setTimeout(p);
            } */
        return t.execute();
    }
    
    /**
     * Execute with standard timeout.
     * 
     * @param s
     * @param v
     * @return
     */
    private Object execute(String s, Vector v) {
        ExeThread t = new ExeThread(client, s, v);
        return t.execute();
    }
}
