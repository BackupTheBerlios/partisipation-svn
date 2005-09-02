package gui;

import java.io.IOException;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.util.Enumeration;
import java.util.Vector;
import org.apache.xerces.parsers.DOMParser;
import org.apache.xmlrpc.WebServer;
import org.apache.xmlrpc.XmlRpc;
import org.apache.xmlrpc.XmlRpcClientLite;
import org.apache.xmlrpc.XmlRpcException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * This class can execute complex scenarios specified in XML to test the GUI.
 *
 * You can use "*" to indicate an unspecified function or parameter value.
 *
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 *
 */
public class Tester {

    Vector expar = new Vector();

    Vector sendpar = new Vector();

    Vector gotpar = new Vector();

    String expfct = new String();

    String sendfct = new String();

    String gotfct = new String();

    XmlRpcClientLite client;

    boolean suspend = true;

    public Tester() {

        try {
            client = new XmlRpcClientLite(InetAddress.getByName(Utils.GUIHOST)
                    .getHostName(), Utils.GUIPORT);
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
    }

    public void traverse(Node node) {
        int type = node.getNodeType();

        if (type == Node.ELEMENT_NODE) {
            if (node.getNodeName().equalsIgnoreCase("print")) {
                // use System.err just to show this info
                // in different color in Eclipse console
                System.err.println("[ " + node.getFirstChild().getNodeValue()
                        + " ]");
            } else if (node.getNodeName().equalsIgnoreCase("sleep")) {
                int pause = Integer.parseInt(node.getFirstChild()
                        .getNodeValue());
                System.out.println("> Sleeping for " + pause + " ms.");
                try {
                    Thread.sleep(pause);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            } else if (node.getNodeName().equalsIgnoreCase("call")) {

                sendfct = node.getAttributes().getNamedItem("method")
                        .getNodeValue();
                setParameters(node, sendpar);

                try {
                    System.out.println("> Calling GUI method " + sendfct + "["
                            + printParameters(sendpar) + "]");

                    client.execute("gui." + sendfct, sendpar);

                } catch (XmlRpcException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }

            } else if (node.getNodeName().equalsIgnoreCase("*")) {
                System.out.println("------- OK: Expected arbitrary "
                        + " function. -------");
                suspend = true;
            } else if (node.getNodeName().equalsIgnoreCase("expect")) {

                expfct = node.getAttributes().getNamedItem("method")
                        .getNodeValue();
                setExpected(node);

                while (suspend) {

                }

                if (expfct.equalsIgnoreCase(gotfct)) {
                    if (parameterMatch()) {
                        System.out
                                .println("------- OK: Expected "
                                        + expfct
                                        + " and got it with proper parameter values. -------");
                        suspend = true;
                    } else {
                        System.out.println("------- TEST FAILED: Expected "
                                + expfct + "[" + printParameters(expar)
                                + "], but got " + gotfct + "["
                                + printParameters(gotpar)
                                + "]. Stop test. -------");
                        System.exit(0);
                    }
                } else {
                    System.out.println("------- TEST FAILED: Expected "
                            + expfct + ", but got " + gotfct
                            + ". Stop test. -------");
                    System.exit(0);
                }
            }
        }

        NodeList children = node.getChildNodes();
        if (children != null) {
            for (int i = 0; i < children.getLength(); i++)
                traverse(children.item(i));
        }
    }

    private boolean parameterMatch() {
        boolean ok = true;
        if (expar.size() == gotpar.size()) {
            Enumeration e = expar.elements();
            Enumeration f = gotpar.elements();
            while (e.hasMoreElements() && ok) {
                String strexp = e.nextElement().toString();
                String strgot = f.nextElement().toString();
                if (strexp.equalsIgnoreCase("*")) {
                    ok = true;
                } else {
                    ok = strexp.equalsIgnoreCase(strgot.toString());
                }
            }
            return ok;
        } else
            return false;
    }

    private String printParameters(Vector v) {
        String s = "";
        if (v.size() > 0) {
            Enumeration e = v.elements();
            while (e.hasMoreElements()) {
                s += e.nextElement().toString() + ",";
            }
            return s.substring(0, s.length() - 1);
        } else
            return s;
    }

    /**
     * Set expected parameters. Note, that they are set only with their String
     * representation, not converted to a proper type.
     *
     * @param node
     */
    private void setExpected(Node node) {
        expar.clear();
        NodeList c = node.getChildNodes();
        if (c != null) {
            for (int i = 0; i < c.getLength(); i++) {
                if (c.item(i).getNodeType() == Node.ELEMENT_NODE) {
                    String s = c.item(i).getFirstChild().getNodeValue();
                    expar.add(s);
                }
            }
        }
    }

    private void setParameters(Node node, Vector par) {
        par.clear();
        NodeList c = node.getChildNodes();

        int k = 0;
        if (c != null) {
            for (int i = 0; i < c.getLength(); i++) {
                if (c.item(i).getNodeType() == Node.ELEMENT_NODE) {
                    String s = c.item(i).getFirstChild().getNodeValue();

                    if (sendfct.equalsIgnoreCase("changeRegStatus")) {
                        if (k == 0)
                            par.add(k, new Integer(s));
                        else
                            par.add(k, new Boolean(s));
                    } else if (sendfct.equalsIgnoreCase("changeCallStatus")
                            || sendfct.equalsIgnoreCase("showUserEvent")) {
                        if (k == 0)
                            par.add(k, new Integer(s));
                        else
                            par.add(k, s);
                    } else if (sendfct.equalsIgnoreCase("incomingCall")) {
                        if (k < 2)
                            par.add(k, new Integer(s));
                        else
                            par.add(k, s);
                    } else if (sendfct.equalsIgnoreCase("setSpeakerVolume")
                            || sendfct.equalsIgnoreCase("setMicroVolume")) {
                        par.add(k, new Double(s));
                    }
                    k++;
                }
            }
        }
    }

    public boolean register(int accId) {
        gotfct = "register";
        gotpar.clear();
        gotpar.add(new Integer(accId));
        suspend = false;
        return true;
    }

    public String registerGui(String address, int port) {
        gotfct = "registerGui";
        gotpar.clear();
        gotpar.add(address);
        gotpar.add(new Integer(port));
        suspend = false;
        return "OK";
    }

    public int makeCall(int accId, String sipaddr) {
        gotfct = "makeCall";
        gotpar.clear();
        gotpar.add(new Integer(accId));
        gotpar.add(sipaddr);
        suspend = false;
        return 1;
    }

    public boolean acceptCall(int callId) {
        gotfct = "acceptCall";
        gotpar.clear();
        gotpar.add(new Integer(callId));
        suspend = false;
        return true;
    }

    public boolean endCall(int callId) {
        gotfct = "endCall";
        gotpar.clear();
        gotpar.add(new Integer(callId));
        suspend = false;
        return true;
    }

    public boolean unregister(int accId) {
        gotfct = "unregister";
        gotpar.clear();
        gotpar.add(new Integer(accId));
        suspend = false;
        return true;
    }

    public double getSpeakerVolume() {
        gotfct = "getSpeakerVolume";
        gotpar.clear();
        suspend = false;
        return 0.7;
    }

    public double getMicroVolume() {
        gotfct = "getMicroVolume";
        gotpar.clear();
        suspend = false;
        return 0.3;
    }

    public Vector accountGetAll() {
        gotfct = "accountGetAll";
        gotpar.clear();
        Vector v = new Vector();
        v.add(new Integer(1));
        v.add(new Integer(2));
        v.add(new Integer(3));
        v.add(new Integer(4));
        suspend = false;
        return v;
    }

    public boolean sendDtmf(String c, int callId) {
        gotfct = "sendDtmf";
        gotpar.clear();
        gotpar.add(c);
        gotpar.add(new Integer(callId));
        suspend = false;
        return true;
    }

    public int accountCreate() {
        gotfct = "accountCreate";
        gotpar.clear();
        suspend = false;
        return 1;
    }

    public boolean accountDelete(int accId) {
        gotfct = "accountDelete";
        gotpar.clear();
        gotpar.add(new Integer(accId));
        suspend = false;
        return true;
    }

    public String accountGet(int accId, String attr) {
        gotfct = "accountGet";
        gotpar.clear();
        gotpar.add(new Integer(accId));
        gotpar.add(attr);
        suspend = false;
        if (attr.equalsIgnoreCase("NAME")) {
            return "My cute SIP account #" + accId + ".";
        } else if (attr.equalsIgnoreCase("USERNAME")) {
            return "anton_huttenlocher";
        } else if (attr.equalsIgnoreCase("DOMAIN")) {
            return "domain.org";
        } else if (attr.equalsIgnoreCase("AUTHUSERNAME")) {
            return "anton";
        } else if (attr.equalsIgnoreCase("PASSWORD")) {
            return "secret";
        } else if (attr.equalsIgnoreCase("DISPLAYNAME")) {
            return "Mr. Anton Huttenlocher";
        } else if (attr.equalsIgnoreCase("OUTBOUNDPROXY")) {
            return "outboundproxy.org";
        } else if (attr.equalsIgnoreCase("REGISTRAR")) {
            return "registrar.org";
        } else if (attr.equalsIgnoreCase("AUTOREGISTER")) {
            return "1";
        } else
            return "";
    }

    public boolean accountSet(int accId, String attr, String value) {
        gotfct = "accountSet";
        gotpar.clear();
        gotpar.add(new Integer(accId));
        gotpar.add(attr);
        gotpar.add(value);
        suspend = false;
        return true;
    }

    public boolean setSpeakerVolume(double level) {
        gotfct = "setSpeakerVolume";
        gotpar.clear();
        gotpar.add(new Double(level));
        suspend = false;
        return true;
    }

    public boolean setMicroVolume(double level) {
        gotfct = "setMicroVolume";
        gotpar.clear();
        gotpar.add(new Double(level));
        suspend = false;
        return true;
    }

    public static void main(String[] args) {

        if (args.length != 1) {
            System.out
                    .println("> Please specify path to your scenario XML file as command line argument. Exit.");
            return;
        } else {

            try {

                DOMParser parser = new DOMParser();
                Tester tester = new Tester();

                // validate scenario against DTD
                parser.setFeature("http://xml.org/sax/features/validation",
                        true);

                parser.parse(args[0]);
                Document document = parser.getDocument();

                System.out.println("> SCENARIO: "
                        + parser.getDocument().getDocumentElement()
                                .getAttribute("name"));

                //  XmlRpc.setDebug(true);
                WebServer server = new WebServer(Utils.COREPORT, InetAddress
                        .getByName(Utils.COREHOST));
                server.start();
                server.addHandler("core", tester);

                tester.traverse(document);

                System.out.println("> SCENARIO COMPLETED.");
                System.exit(0);

            } catch (UnknownHostException e) {
                e.printStackTrace();
            } catch (SAXException e) {
                System.out.println("> Your XML file seems to be invalid.");
            } catch (IOException e) {
                System.out.println("> No XML file found. Check your path.");
            }
        }
    }
}
