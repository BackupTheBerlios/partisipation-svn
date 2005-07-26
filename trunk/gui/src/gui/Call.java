package gui;

/**
 * 
 * 
 * @author Oleksiy Reznikov
 * @author Anton Huttenlocher
 *
 */
public class Call {
    
    public int id;
    public int account;
    public String sipurl;
    public String name;
    
    public Call(int callId, int acc, String sipAddr, String dispName) {
        id = callId;
        account = acc;
        sipurl = sipAddr;
        name = dispName;
    }
}
