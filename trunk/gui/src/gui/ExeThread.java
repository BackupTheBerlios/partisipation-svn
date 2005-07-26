package gui;

/**
 * This class prevents the GUI from hanging if a XML-RPC call cannot return due
 * to absence of the core or other reasons by managing a separate thread for 
 * each call, which is interrupted either after a timeout or immediately after 
 * a successful return of the RPC call.
 * 
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 *  
 */

import java.io.IOException;
import java.util.Vector;

import javax.swing.JOptionPane;

import org.apache.xmlrpc.XmlRpcClientLite;
import org.apache.xmlrpc.XmlRpcException;

class ExeThread {

    private String func;

    private Vector par;

    private Object result = null;
    
    private int pause = 60000;
    
    private XmlRpcClientLite client = null;
    
    private String XMLRPC_CALL_TIMEOUT = "XML-PRC CALL TIMEOUT. \n";
    
    private Thread watchthread = null;
    
    private boolean finished = false;
    
    public ExeThread(XmlRpcClientLite c, String f, Vector p) {
        client = c;
        func = f;
        par = p;
    }

    public Object execute() {
        
        watchthread = new WatchThread();
        watchthread.start();
        Thread exe = new ExecuteThread();
        exe.start();
      
        while (!finished) {
            
        }
        
        if (result == null) {
            JOptionPane.showMessageDialog(null, XMLRPC_CALL_TIMEOUT, "ERROR",
                    JOptionPane.ERROR_MESSAGE);
        } 
        return result;
    }

    public void setTimeout(int timeout) {
        pause = timeout;
    }
    
    class ExecuteThread extends Thread {
        public void run() {
            try {
                result = client.execute(func, par);
                watchthread.interrupt();
            } catch (XmlRpcException e1) {
                	result = null;
                	finished = true;
            } catch (IOException e1) {
                	result = null;
                	finished = true;
            }
        }
    }
    
    class WatchThread extends Thread {
        public void run() {
            try {
                sleep(pause);
            } catch (InterruptedException e) {
                finished = true;
            }
            finished = true;
        }
    }
}