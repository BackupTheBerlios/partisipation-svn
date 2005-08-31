package gui;

/**
 *
 *
 * @author Oleksiy Reznikov
 * @author Anton Huttenlocher
 *
 */
public class Account {

    public int id;
    public boolean registered;

    public Account(int i, boolean r) {
        id = i;
        registered = r;
    }

    public void setRegistered(boolean r) {
        registered = r;
    }

}
