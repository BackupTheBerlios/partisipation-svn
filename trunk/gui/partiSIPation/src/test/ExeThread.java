package test;

import java.io.IOException;
import java.util.Vector;

import org.apache.xmlrpc.XmlRpcClientLite;
import org.apache.xmlrpc.XmlRpcException;

class ExeThread extends Thread {

    private String func;

    private Vector par;

    private Object result = null;
    
    private int pause = 5000;
    
    private XmlRpcClientLite client = null;

    public ExeThread(XmlRpcClientLite c, String f, Vector p) {
        client = c;
        func = f;
        par = p;
    }

    public Object execute() {

        this.start();
        try {
            Thread.sleep(pause);
        } catch (InterruptedException e) {
            return result;
        }
        return result;
    }


    public void run() {
        try {
            result = client.execute(func, par);
            this.interrupt();
        } catch (XmlRpcException e1) {
            result = null;
        } catch (IOException e1) {
            result = null;
        }
    }
    
    public void setTimeout(int timeout) {
        pause = timeout;
    }
}

