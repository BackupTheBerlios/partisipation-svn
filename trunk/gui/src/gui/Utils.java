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

    public static String getTimestamp() {
        Locale currentLocale = new Locale("de", "DE");
        DateFormat timestamp = 
            DateFormat.getDateTimeInstance(DateFormat.SHORT,
                                           DateFormat.DEFAULT,
                                           currentLocale);
        return timestamp.format(new Date());
    }
    
}
