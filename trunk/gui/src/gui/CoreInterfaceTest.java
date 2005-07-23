package gui;

import java.io.IOException;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.util.Enumeration;
import java.util.Vector;
import org.apache.xmlrpc.WebServer;
import org.apache.xmlrpc.XmlRpcClientLite;
import org.apache.xmlrpc.XmlRpcException;
import junit.framework.Assert;
import junit.framework.TestCase;

/**
 * JUnit test case for the core interface.
 * 
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */
public class CoreInterfaceTest extends TestCase {

    XmlRpcClientLite client;

    WebServer server;

    Vector params = new Vector();

    String GUI_HOST = "192.168.0.1";

    String CORE_HOST = "192.168.0.1";

    int GUI_PORT = 8888;

    int CORE_PORT = 7777;

    public CoreInterfaceTest() {
        try {

            client = new XmlRpcClientLite(InetAddress.getByName(CORE_HOST)
                    .getHostName(), CORE_PORT);

            /*
             * server = new WebServer(GUI_PORT,
             * InetAddress.getByName(GUI_HOST)); server.start();
             * server.addHandler("gui", new RequestProcessor());
             */

        } catch (MalformedURLException e) {
            fail("Failed: create client.");
        } catch (UnknownHostException e) {
            fail("Failed: create client.");
        }
    }

    public void testRegisterGui() {
        params.clear();
        params.add(GUI_HOST);
        params.add(new Integer(GUI_PORT));
        String expected = "OK";
        try {
            Assert.assertTrue(expected.equals((String) client.execute(
                    "core.registerGui", params)));
        } catch (XmlRpcException e) {
            fail("Failed: registerGui");
        } catch (IOException e) {
            fail("Failed: registerGui");
        }
    }

    public void testRegister() {
        params.clear();
        params.add(new Integer(0));
        try {
            Assert.assertTrue(((Boolean) client
                    .execute("core.register", params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: register");
        } catch (IOException e) {
            fail("Failed: register");
        }
    }

    public void testUnregister() {
        params.clear();
        params.add(new Integer(0));
        try {
            Assert.assertTrue(((Boolean) client.execute("core.unregister",
                    params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: unregister");
        } catch (IOException e) {
            fail("Failed: unregister");
        }
    }

    public void testMakeCall() {
        params.clear();
        params.addElement(new Integer(0));
        params.addElement("MUSTERMANN");
        try {
            Assert
                    .assertTrue(client.execute("core.makeCall", params) instanceof Integer);
        } catch (XmlRpcException e) {
            fail("Failed: makeCall");
        } catch (IOException e) {
            fail("Failed: makeCall");
        }
    }

    public void testEndCall() {
        params.clear();
        params.add(new Integer(0));
        try {
            Assert
                    .assertTrue(((Boolean) client.execute("core.endCall",
                            params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: endCall");
        } catch (IOException e) {
            fail("Failed: endCall");
        }
    }

    public void testAcceptCall() {
        params.clear();
        params.add(new Integer(0));
        try {
            Assert.assertTrue(((Boolean) client.execute("core.acceptCall",
                    params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: acceptCall");
        } catch (IOException e) {
            fail("Failed: acceptCall");
        }
    }

    public void testSetSpeakerVolume() {
        params.clear();
        params.add(new Double(0.5));
        try {
            Assert.assertTrue(((Boolean) client.execute(
                    "core.setSpeakerVolume", params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: setSpeakerVolume");
        } catch (IOException e) {
            fail("Failed: setSpeakerVolume");
        }
    }

    public void testGetSpeakerVolume() {
        params.clear();
        try {
            Assert
                    .assertTrue(client.execute("core.getSpeakerVolume", params) instanceof Double);
        } catch (XmlRpcException e) {
            fail("Failed: getSpeakerVolume");
        } catch (IOException e) {
            fail("Failed: getSpeakerVolume");
        }
    }

    public void testSetMicroVolume() {
        params.clear();
        params.add(new Double(0.5));
        try {
            Assert.assertTrue(((Boolean) client.execute("core.setMicroVolume",
                    params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: setMicroVolume");
        } catch (IOException e) {
            fail("Failed: setMicroVolume");
        }
    }

    public void testGetMicroVolume() {
        params.clear();
        try {
            Assert
                    .assertTrue(client.execute("core.getMicroVolume", params) instanceof Double);
        } catch (XmlRpcException e) {
            fail("Failed: getMicroVolume");
        } catch (IOException e) {
            fail("Failed: getMicroVolume");
        }
    }

    public void testSendDtmf() {
        params.clear();
        params.add(new String("#"));
        params.add(new Integer(0));
        try {
            Assert.assertTrue(((Boolean) client
                    .execute("core.sendDtmf", params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: sendDtmf");
        } catch (IOException e) {
            fail("Failed: sendDtmf");
        }
    }

    public void testAccountGetAll() {
        params.clear();
        try {
            Object o = client.execute("core.accountGetAll", params);
            Assert.assertTrue(o instanceof Vector);
            Vector v = (Vector) o;
            Enumeration e = v.elements();
            while (e.hasMoreElements()) {
                Object n = e.nextElement();
                Assert.assertTrue(n instanceof Integer);
            }
        } catch (XmlRpcException e) {
            fail("Failed: accountGetAll");
            e.printStackTrace();
        } catch (IOException e) {
            fail("Failed: accountGetAll");
            e.printStackTrace();
        }
    }

    public void testAccountSet() {
        params.clear();
        params.addElement(new Integer(0));
        params.addElement("ATTRIBUTE");
        params.addElement("VALUE");
        try {
            Assert.assertTrue(((Boolean) client.execute("core.accountSet",
                    params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: accountSet");
        } catch (IOException e) {
            fail("Failed: accountSet");
        }
    }

    public void testAccountGet() {
        params.clear();
        params.addElement(new Integer(0));
        params.addElement("MUSTERMANN");
        try {
            Assert
                    .assertTrue(client.execute("core.accountGet", params) instanceof String);
        } catch (XmlRpcException e) {
            fail("Failed: accountGet");
        } catch (IOException e) {
            fail("Failed: accountGet");
        }
    }

    public void testAccountCreate() {
        params.clear();
        try {
            Assert
                    .assertTrue(client.execute("core.accountCreate", params) instanceof Integer);
        } catch (XmlRpcException e) {
            fail("Failed: accountCreate");
        } catch (IOException e) {
            fail("Failed: accountCreate");
        }
    }

    public void testAccountDelete() {
        params.clear();
        params.add(new Integer(0));
        try {
            Assert.assertTrue(((Boolean) client.execute("core.accountDelete",
                    params)).booleanValue());
        } catch (XmlRpcException e) {
            fail("Failed: accountDelete");
        } catch (IOException e) {
            fail("Failed: accountDelete");
        }
    }
}
