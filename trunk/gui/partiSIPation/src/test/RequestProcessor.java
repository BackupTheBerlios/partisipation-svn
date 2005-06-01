package test;

/**
 * Provides methods called by a client via XML-RPC and writes an appropriate
 * output to the console.
 *
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 *
 */
public class RequestProcessor {

    public static String register2(String fullname, String username,
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
    public static boolean register(int accId) {
         System.out.println("####### [" + Utils.getTimestamp()
                 + "] PROC-CALL: REGISTER #######");
         System.out.println("AccountId: " + accId);

         return true;
     }

     public static boolean registerGui(String adress, int port) {
        System.out.println("####### [" + Utils.getTimestamp()
                + "] PROC-CALL: REGISTER GUI #######");

        return true;
    }


    public static boolean makeCall(int accId, String number) {
          System.out.println("####### [" + Utils.getTimestamp()
                + "] PROC-CALL: MAKE CALL #######");
        return true;
    }
    public static boolean acceptCall(int callId) {
          System.out.println("####### [" + Utils.getTimestamp()
                  + "] PROC-CALL: REGISTER #######");
          System.out.println("Accepted " + callId);

          return true;
      }

    public static String makeConf(String id) {
            System.out.println("####### [" + Utils.getTimestamp()
                    + "] PROC-CALL: MAKE CONFERENCE #######");
 // todo: procedure call in core
            return "CONFERENCE MODE: user with id: " + id + " joined this session";
        }


    public static boolean endCall(int callId) {
      System.out.println("####### [" + Utils.getTimestamp()
                    + "] Terminating Call #######");
        return true;
    }

    public static boolean unregister(int accId) {
           System.out.println("####### [" + Utils.getTimestamp()
                   + "] PROC-CALL: UNREGISTER ####### " + accId);
// todo: procedure call in core
  return true;
       }

}
