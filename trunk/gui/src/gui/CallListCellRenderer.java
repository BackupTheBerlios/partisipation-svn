package gui;

import java.awt.Color;
import java.awt.Component;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.ListCellRenderer;

/**
 *
 *
 * @author Anton Huttenlocher
 * @author Oleksiy Reznikov
 *
 */
public class CallListCellRenderer extends JLabel implements ListCellRenderer {

    Color selColor = new Color(0, 0, 128);

    public CallListCellRenderer() {
        setOpaque(true);
    }


    public Component getListCellRendererComponent(JList list, Object value,
            int index, boolean isSelected, boolean cellHasFocus) {

        if (index == -1) {
            int selected = list.getSelectedIndex();
            if (selected == -1) {
                return this;
            } else {
                index = selected;
            }
        }

        if (isSelected) {
            setBackground(list.getSelectionBackground());
            setForeground(list.getSelectionForeground());
        } else {
            setBackground(list.getBackground());
            setForeground(list.getForeground());
        }

        ImageIcon icon = (ImageIcon)value;
        setText(icon.getDescription());
        setIcon(icon);

        return this;

    }
}
