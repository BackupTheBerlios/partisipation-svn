package test;

import java.util.Vector;

/**
 * Provides methods called by a client via XML-RPC and writes an appropriate
 * output to the console.
 *
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 *
 */
public class RequestProcessor {

    // complete dummy CORE interface
    
    public static boolean register(int accId) {
         return true;
     }

     public static String registerGui(String address, int port) {
         return "OK";
    }


    public static int makeCall(int accId, String number) {
        return 1;
    }
    
    public static boolean acceptCall(int callId) {
          return true;
    }

    public static boolean endCall(int callId) {
        return true;
    }

    public static boolean unregister(int accId) {
       	return true;
    }
    
    public static double getSpeakerVolume() {
        return 0.7;
    }

    public static double getMicroVolume() {
        return 0.3;
    }
   
    public static Vector accountGetAll() {
        Vector v = new Vector();
        v.add(new Integer(1));
        v.add(new Integer(2));
        v.add(new Integer(3));
        v.add(new Integer(4));
        return v;
    }
    
    public static boolean sendDtmf(String c, int callId) {
        return true;
    }
    
    public static int accountCreate() {
        return 1;
    }
    
    public static boolean accountDelete(int accId) {
        return true;
    }
    
    public static String accountGet(int accId, String attr) {
        if (attr.equalsIgnoreCase("NAME")) {
            return "My cute SIP account #" + accId+".";
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
            return "yes";
        } else return "";      
    }
    
    public static boolean setValue(int accId, String attr, String value) {
        return true;
    }
    
    // complete dummy GUI interface
    
    public static boolean changeRegStatus(int accountId, boolean registered) {
        return true;
    }
    
    public static boolean changeCallStatus(int callId, String callStatus) {
        return true;
    }
    
    public static boolean showUserEvent(int accountId, String category, String title, String message, String details) {
        return true;
    }
    
    public static boolean registerCore() {
        return true;
    }
    
    public static boolean incomingCall(int accountId,int callId, String sipUri, String displayName) {
        return true;
    } 
    
    public static boolean setSpeakerVolume(double level) {
        return true;
    }
    
    public static boolean setMicroVolume(double level) {
        return true;
    }
}
