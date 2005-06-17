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
    
    private static String GUI_HOST = "192.168.0.3";
    private static int GUI_PORT = 8888;

    public static void main(String[] args) {
        // turn XML-view of method calls on 
        XmlRpc.setDebug(true);
        try {
            WebServer server = new WebServer(GUI_PORT, InetAddress.getByName(GUI_HOST));
            server.start();
            server.addHandler("gui", new RequestProcessor());
        } catch (UnknownHostException uhe) {

        }
    }
}
