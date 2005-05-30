package test;

import java.io.IOException;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.util.Vector;
import org.apache.xmlrpc.XmlRpcClientLite;
import org.apache.xmlrpc.XmlRpcException;

/**
 * This class tests the interface offered by GUI to the core by making some 
 * XML-RPC calls.
 * 
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 *
 */
public class TestClient {

    public static void main(String[] args) {
        try {
            XmlRpcClientLite client = new XmlRpcClientLite(InetAddress.getLocalHost()
                    .getHostName(), 8888);
            Vector params = new Vector();
            client.execute("gui.registerCore",params);
            params.clear();
            params.add(new Integer(1));
            params.add(new Boolean(true));
            client.execute("gui.changeRegStatus",params);
        } catch (MalformedURLException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (XmlRpcException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
