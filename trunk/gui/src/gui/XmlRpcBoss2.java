package gui;

import java.net.UnknownHostException;
import org.apache.xmlrpc.XmlRpcClientLite;
import java.util.Vector;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.io.IOException;
import org.apache.xmlrpc.XmlRpcException;

/**
 * <p>XmlRpcBoss2.java</p>
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */
public class XmlRpcBoss2 {

  static String GUI_HOST = "192.168.0.1";

  static int GUI_PORT = 8888;

  public static void main(String[] args) throws IOException, XmlRpcException {
    try {

      Vector params = new Vector();
      params.clear();
      String method;

      if (args[0].equalsIgnoreCase("changeRegStatus")) {
        method = "gui.changeRegStatus";
        params.addElement(new Integer(args[1]));
        params.addElement(new Boolean(args[2]));
      }
      else if (args[0].equalsIgnoreCase("changeCallStatus")) {
        method = "gui.changeCallStatus";
        params.addElement(new Integer(args[1]));
        params.addElement(new String(args[2]));
      }
      else if (args[0].equalsIgnoreCase("showUserEvent")) {
        method = "gui.showUserEvent";
        params.addElement(new Integer(args[1]));
        params.addElement(new String(args[2]));
        params.addElement(new String(args[3]));
        params.addElement(new String(args[4]));
        params.addElement(new String(args[5]));
      }
      else if (args[0].equalsIgnoreCase("registerCore")) {
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
      }
      else if (args[0].equalsIgnoreCase("setMicroVolume")) {
        method = "gui.setMicroVolume";
        params.addElement(new Double(args[1]));

      }
      else {
        System.out.println("No provides no such method");
        return;
      }

      XmlRpcClientLite client = new XmlRpcClientLite(InetAddress.getByName(
          GUI_HOST).getHostName(), GUI_PORT);

      ExeThread t = new ExeThread(client, method, params);
      t.setTimeout(300);
      t.execute();

    }
    catch (MalformedURLException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
    catch (UnknownHostException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
    catch (IOException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();

    }

  }
}
