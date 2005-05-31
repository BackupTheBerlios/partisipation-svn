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
    public static String register(int accId) {
         System.out.println("####### [" + Utils.getTimestamp()
                 + "] PROC-CALL: REGISTER #######");
         System.out.println("AccountId: " + accId);

         return "";
     }

     public static String registerGui(String adress, int port) {
        System.out.println("####### [" + Utils.getTimestamp()
                + "] PROC-CALL: REGISTER GUI #######");

        return "REGISTER GUI OK";
    }


    public static String makeCall(int accId, String number) {
          System.out.println("####### [" + Utils.getTimestamp()
                + "] PROC-CALL: MAKE CALL #######");
        return "callId"; //todo
    }
    public static String acceptCall(int callId) {
          System.out.println("####### [" + Utils.getTimestamp()
                  + "] PROC-CALL: REGISTER #######");
          System.out.println("Accepted " + callId);

          return "";
      }

    public static String makeConf(String id) {
            System.out.println("####### [" + Utils.getTimestamp()
                    + "] PROC-CALL: MAKE CONFERENCE #######");
 // todo: procedure call in core
            return "CONFERENCE MODE: user with id: " + id + " joined this session";
        }


    public static String endCall(int callId) {
      System.out.println("####### [" + Utils.getTimestamp()
                    + "] Terminating Call #######");
        return "";
    }

    public static String unregister(int accId) {
           System.out.println("####### [" + Utils.getTimestamp()
                   + "] PROC-CALL: UNREGISTER ####### " + accId);
// todo: procedure call in core
  return "";
       }

}
