package gui;

import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.XMLReader;
import org.xml.sax.Attributes;
import org.apache.xerces.parsers.SAXParser;

class ownParser extends DefaultHandler {

    boolean worth = false;	
    boolean news = false;
    
    public void startElement(String s,String s1,String s2,Attributes a) {
        worth = (s1.equalsIgnoreCase("target"));
        if (worth)
       // System.out.println("Target found");
        if (s1.equalsIgnoreCase("item")) news = true;
    }
    
    public void characters(char ch[], int start, int length) {
        if (news) {
        if (worth) {
            System.out.println(new String(ch, start, length));
        }
        }
    }
    
    public void processingInstruction(String target, String data) {
        System.out.println("TARGET: "+target+" / DATA: "+data);
    }
    
    public void endElement(String s, String s1, String s2) {
        if (s1.equalsIgnoreCase("item")) news = false;
    }

}

class Tester {
    public static void main(String[] args) {
        System.out.println("Hello");
        try {
            XMLReader reader = new SAXParser();
            ownParser h = new ownParser();
            reader.setContentHandler(h);
            reader.parse("D:/Programme/eclipse/workspace/trunk/gui/partiSIPation/build.xml");
         //   h.processingInstruction("")
            System.out.println("Bye");
        } catch(Exception e) {
            e.printStackTrace();
        }

    }

}
