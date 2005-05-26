package test;

import java.net.InetAddress;
import java.net.UnknownHostException;
import org.apache.xmlrpc.WebServer;
import org.apache.xmlrpc.XmlRpc;

/**
 * Runs a tiny web server designed to handle XML-RPC requests.
 * 
 * @author Anton Huttenlocher
 *
 */
public class Server {

    public static void main(String[] args) {
        // turn XML-view of method calls on 
        XmlRpc.setDebug(true);
        try {
            WebServer server = new WebServer(7777, InetAddress.getLocalHost());
            server.start();
            server.addHandler("sip", new RequestProcessor());
        } catch (UnknownHostException uhe) {

        }
    }
}
