package gui;

import java.net.UnknownHostException;
import org.apache.xmlrpc.WebServer;
import org.apache.xmlrpc.XmlRpcClientLite;
import org.apache.xmlrpc.XmlRpcException;

import java.util.Vector;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

/**
 * This class allows to run GUI test scenarios by simulating core activity. The
 * control is done by sending arbitrarily parameterized XML-RPC calls from the
 * console thus shell scripts can be used to do the job.
 * 
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */
public class XmlRpcBoss2 {

    private static String HOST = "192.168.0.1";

    private static int GUI_PORT = 8888;

    private static int CORE_PORT = 7777;
    
    public static void main(String[] args) {
        
        Vector params = new Vector();

        try {
            
            XmlRpcClientLite client = new XmlRpcClientLite(InetAddress.getByName(
                    HOST).getHostName(), GUI_PORT);

                params.clear();
                String method;

                 if (args[0].equalsIgnoreCase("changeRegStatus")) {
                     method = "gui.changeRegStatus";
                     params.addElement(new Integer(args[1]));
                     params.addElement(new Boolean(args[2]));
                 } else if (args[0].equalsIgnoreCase("changeCallStatus")) {
                     method = "gui.changeCallStatus";
                     params.addElement(new Integer(args[1]));
                     params.addElement(new String(args[2]));
                 } else if (args[0].equalsIgnoreCase("showUserEvent")) {
                     method = "gui.showUserEvent";
                     params.addElement(new Integer(args[1]));
                     params.addElement(new String(args[2]));
                     params.addElement(new String(args[3]));
                     params.addElement(new String(args[4]));
                     params.addElement(new String(args[5]));
                 } else if (args[0].equalsIgnoreCase("registerCore")) {
                     method = "gui.registerCore";
                 }
                 else if (args[0].equalsIgnoreCase("incomingCall")) {
                    method = "gui.incomingCall";
                    params.addElement(new Integer(args[1]));
                    params.addElement(new Integer(args[2]));
                    params.addElement(new String(args[3]));
                    params.addElement(new String(args[4]));
                }

                else if (args[0].equalsIgnoreCase("setSpeakerVolume")) {
                    method = "gui.setSpeakerVolume";
                    params.addElement(new Double(args[1]));
                } else if (args[0].equalsIgnoreCase("setMicroVolume")) {
                    method = "gui.setMicroVolume";
                    params.addElement(new Double(args[1]));

                } else {
                    System.out.println("No such method");
                    return;
                }

               
                ExeThread t = new ExeThread(client, method, params);
                t.setTimeout(100000000);
                t.execute();

            //}
        } catch (MalformedURLException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } 
    }
}
