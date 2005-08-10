package gui;

/**
 * Stores contact information for a person from the address book.
 *
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 *
 */
public class Contact {

    public String name;
    public String sipurl;

    public Contact(String dispName, String sipAddr) {
        sipurl = sipAddr;
        name = dispName;
    }
}
