package test;

import java.net.Socket;
import java.io.OutputStream;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.net.UnknownHostException;

/**
 * <p>XmlRpcBoss.java</p>
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 */
public class XmlRpcBoss {

    public static void main(String[] args) {

        Socket newConnection = null;

        try {

            newConnection = new Socket("192.168.0.1", 8888);

            System.out.println("Connected to " + newConnection.getInetAddress()
                               + ":" + newConnection.getPort());

            OutputStream outStream = newConnection.getOutputStream();
            BufferedWriter out = new BufferedWriter(new OutputStreamWriter(outStream));
                
            
            String methodCall = "";
            
            if (args[0].equalsIgnoreCase("changeCallStatus")) {
                methodCall = "<methodName>gui.changeCallStatus</methodName>"
                    +"<params><param><value><i4>"
                    +args[1]
                    +"</i4></value></param><param><value><string>"
                    +args[2]
                    +"</string></value></param></params>";
            } else if (args[0].equalsIgnoreCase("changeCallStatus")) {	
                
            } else if (args[0].equalsIgnoreCase("showUserEvent")) {	
                
            }
            else if (args[0].equalsIgnoreCase("registerCore")) {	
                methodCall = "<methodName>gui.registerCore</methodName>"
                    +"<params></params>";
            }
            else if (args[0].equalsIgnoreCase("incomingCall")) {
                methodCall = "<methodName>gui.incomingCall</methodName>"
                        + "<params><param><value><i4>" + args[1]
                        + "</i4></value></param>" + "<param><value><i4>"
                        + args[2] + "</i4></value></param>"
                        + "<param><value><string>" + args[3]
                        + "</string></value></param>"
                        + "<param><value><string>" + args[4]
                        + "</string></value></param>" + "</params>";
            }
            else if (args[0].equalsIgnoreCase("setSpeakerVolume")) {	
                methodCall = "<methodName>gui.setSpeakerVolume</methodName>"
                    +"<params><param><value><double>"
                    +args[1]
                    +"</double></value></param></params>";
            }
            else if (args[0].equalsIgnoreCase("setMicroVolume")) {	
                
            } else {
                System.out.println("No provides no such method");
                return;
            }
            
            String content = "<?xmlversion=\"1.0\"?><methodCall>"+methodCall+"</methodCall>";

         String header = "POST /RPC2 HTTP/1.0\n"+
         		"User-Agent: SomeAgent/1.0 (Linux)\n"+
         		"Host: laptop\n"+
         		"Content-Type: text/xml\n"+
         		"Content-length:" + content.length() +
         		"\n"+"\r\n";

		
		out.write(header + content);

                out.flush();

            out.close();
            outStream.close();
            
            newConnection.close();

        }
        catch (UnknownHostException uhe) {
            // error finding host
            System.err.println("Host address could not be resolved.");
        }
        catch (IOException ioe) {
            // if an I/O problem occured
            System.err.println("I/O error: " + ioe.getMessage());
        }
        catch (Exception e) {
            // if any other problem occured, print out error message
            System.err.println("Error: " + e.getMessage());
        }
    }
    
}
