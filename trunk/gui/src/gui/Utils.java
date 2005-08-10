package gui;

import java.text.DateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * This class provides reusable static utilities.
 *
 * @author Anton Huttenlocher
 *
 */
public class Utils {

    public final static String GUIHOST = "192.168.0.1";

    public final static String COREHOST = "192.168.0.1";

    public final static int GUIPORT = 8888;

    public final static int COREPORT = 7777;

    public static String getTimestamp() {
        Locale currentLocale = new Locale("de", "DE");
        DateFormat timestamp =
            DateFormat.getDateTimeInstance(DateFormat.SHORT,
                                           DateFormat.DEFAULT,
                                           currentLocale);
        return timestamp.format(new Date());
    }

}
