package test;

/**
 * Provides methods called by a client via XML-RPC and writes an appropriate
 * output to the console.
 * 
 * @author Anton Huttenlocher
 *  
 */
public class RequestProcessor {
    
    public static String register(String fullname, String username,
            String password, String domain) {
        System.out.println("####### [" + Utils.getTimestamp()
                + "] PROC-CALL: REGISTER #######");
        System.out.println("Full Name: " + fullname);
        System.out.println("Username: " + username);
        System.out.println("Domain: " + domain);
        System.out.println("Password: " + password);
        
        // the client should print out this response, if the XML-RPC
        // call was successful 
        return "REGISTER OK";
    }

    public static String makeCall(String number) {
        System.out.println("####### [" + Utils.getTimestamp()
                + "] PROC-CALL: MAKE CALL #######");
        return "CALLING " + number + "...";
    }

    public static String endCall() {
        return "TERMINATING CALL ...";
    }
}
