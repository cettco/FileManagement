/*
 * File:Main.java
 * Function:main function
 */
import java.awt.Color;

import javax.swing.JFrame;
public class Main {

	public static void main(String[]args){
		MainFrame frame = new MainFrame(1024);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);
	}
}
