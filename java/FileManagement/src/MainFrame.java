/*
 * File:MainFrame.java
 * Function:����������
 */
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.border.Border;
import javax.swing.border.LineBorder;
import javax.swing.table.DefaultTableModel;


public class MainFrame extends JFrame{
	private static final long serialVersionUID = 1L;
	private static int totalSize = 0;
	JButton back;
	JButton forward;
	JLabel posLabel;
	Display dd;		//��ʾ����
	JLabel	UsedSpaceDisplay;
	JLabel FreeSpaceDisplay;
	static int UsedSpace = 0;
	static int FreeSpace = 0;
	JPopupMenu popupMenu;
	public MainFrame(int size){
		setLayout(null);
		setBackground(Color.pink);
		setSize(600, 600);
		setTitle("File System");
		setResizable(false);
		this.totalSize = size;
		this.FreeSpace = totalSize;
		JMenuBar bar = new JMenuBar();
		setJMenuBar(bar);
		JMenu fileMenu = new JMenu("File");
		JMenuItem format = new JMenuItem("Format");
		format.addActionListener(new formatAction());
		JMenuItem createFolder = new JMenuItem("Create Folder",new ImageIcon("img/folder.png"));
		createFolder.addActionListener(new createSubDirAction());
		JMenuItem deleteFile = new JMenuItem("Delete File",new ImageIcon("img/delete.png"));
		deleteFile.addActionListener(new deleteFileAction());
		JMenuItem createFile = new JMenuItem("Create File",new ImageIcon("img/file.png"));
		createFile.addActionListener(new createFileAtion());
		JMenuItem deleteFolder = new JMenuItem("Delete Folder",new ImageIcon("img/delete.png"));
		deleteFolder.addActionListener(new deleteSubDirAction());
		JMenuItem property = new JMenuItem("Property",new ImageIcon("img/property.png"));
		property.addActionListener(new propertyAction());
		JMenuItem open = new JMenuItem("Open",new ImageIcon("img/open.png"));
		open.addActionListener(new openAction());
		fileMenu.add(format);
		fileMenu.add(createFolder);
		fileMenu.add(deleteFolder);
		fileMenu.add(createFile);
		fileMenu.add(deleteFile);
		fileMenu.add(open);
		fileMenu.add(property);
		bar.add(fileMenu);
		JMenu help = new JMenu("Help");
		JMenuItem helpmsg = new JMenuItem("Help",new ImageIcon("img/help.png"));
		helpmsg.addActionListener(new instructionAction());
		help.add(helpmsg);
		bar.add(help);
		
		dd = new Display();
		add(dd);
		UsedSpaceDisplay = new JLabel(String.valueOf(UsedSpace));
		UsedSpaceDisplay.setBorder(new LineBorder(Color.red));
		UsedSpaceDisplay.setBounds(150, 50, 150, 30);
		UsedSpaceDisplay.setFont(new Font("", Font.HANGING_BASELINE, 14));
		add(UsedSpaceDisplay);
		JLabel usedlLabel = new JLabel("Used Size:");
		usedlLabel.setBorder(new LineBorder(Color.red));
		usedlLabel.setBounds(50, 50, 100, 30);
		add(usedlLabel);
		FreeSpaceDisplay = new JLabel(String.valueOf(FreeSpace));
		FreeSpaceDisplay.setBorder(new LineBorder(Color.red));
		FreeSpaceDisplay.setBounds(420, 50, 150, 30);
		FreeSpaceDisplay.setFont(new Font("", Font.HANGING_BASELINE, 14));
		add(FreeSpaceDisplay);
		JLabel freeLabel = new JLabel("Free Size:");
		freeLabel.setBorder(new LineBorder(Color.red));
		freeLabel.setBounds(320, 50, 100, 30);
		add(freeLabel);
		

		back = new JButton("Back");
		back.addActionListener(new backAction());
		back.setBounds(250, 130, 100, 30);
		add(back);
		
		posLabel = new JLabel("Position:");
		posLabel.setBounds(100, 0, 400, 30);
		posLabel.setBackground(Color.orange);
		posLabel.setBorder(new LineBorder(Color.blue));
		add(posLabel);
		
		popupMenu = new JPopupMenu();

		JMenuItem popcreateFolder = new JMenuItem("Create Folder");
		popcreateFolder.addActionListener(new createSubDirAction());
		popupMenu.add(popcreateFolder);
		JMenuItem popdeleteFile = new JMenuItem("Delete File");
		popdeleteFile.addActionListener(new deleteFileAction());
		popupMenu.add(popdeleteFile);
		JMenuItem popcreateFile = new JMenuItem("Create File");
		popcreateFile.addActionListener(new createFileAtion());
		popupMenu.add(popcreateFile);
		JMenuItem popdeleteFolder = new JMenuItem("Delete Folder");
		popupMenu.add(popdeleteFolder);
		popdeleteFolder.addActionListener(new deleteSubDirAction());
		JMenuItem popproperty = new JMenuItem("Property");
		popproperty.addActionListener(new propertyAction());
		popupMenu.add(popproperty);
		JMenuItem popopen = new JMenuItem("Open");
		popopen.addActionListener(new openAction());
		popupMenu.add(popopen);
		dd.add(popupMenu);
		dd.setComponentPopupMenu(popupMenu);
		readFile();
	}

	private void readFile()
	{
		Queue <MyFolder> waitingFolders = new ConcurrentLinkedQueue<MyFolder>();
		int fileNumber =0,folderNumber = 0,fileSize = 0,contentSize = 0;
		String name = "";
		char[] buffer = new char[512];
		try
		{
			MyFolder tempFolder = dd.RootFolder;
			BufferedReader br = new BufferedReader(
					new InputStreamReader(new FileInputStream("data.txt")));
			waitingFolders.add(tempFolder);
			//���ļ�
			do
			{
				tempFolder = waitingFolders.peek();
				dd.CurrentFolder = tempFolder;
				folderNumber = Integer.parseInt(br.readLine());
				for(int i=0;i<folderNumber;i++)
				{
					name = br.readLine();
					dd.CreateFolder(name);
				}
				fileNumber = Integer.parseInt(br.readLine());
				for(int i=0;i<fileNumber;i++)
				{
					name = br.readLine();
					dd.CreateFile(name);
					fileSize = Integer.parseInt(br.readLine());
					dd.CurrentFolder.FileList.get(i).fileSize = fileSize;
					contentSize = Integer.parseInt(br.readLine());
					br.read(buffer, 0, contentSize);
					dd.CurrentFolder.FileList.get(i).fileContent = turnToString(buffer,contentSize);
					dd.updateTable();
					br.skip(2l);
					UsedSpace += fileSize;
				}
				
				for(int i=0;i<tempFolder.FolderList.size();i++)
				{
					waitingFolders.add(tempFolder.FolderList.get(i));
				}
				waitingFolders.remove();
			}while(waitingFolders.peek()!=null);

			br.close();
			dd.CurrentFolder = dd.RootFolder;
			((DefaultTableModel)dd.DTable.getModel()).getDataVector().removeAllElements();
			for(int i=0;i<(dd.RootFolder.FileList.size()+dd.RootFolder.FolderList.size());i++)
				((DefaultTableModel)dd.DTable.getModel()).addRow(new Object[2]);
			((DefaultTableModel)dd.DTable.getModel()).fireTableDataChanged();
			dd.updateTable();
			UsedSpaceDisplay.setText(String.valueOf(UsedSpace));
			FreeSpace = totalSize - UsedSpace;
			FreeSpaceDisplay.setText(String.valueOf(FreeSpace));
		}
		catch(Exception e)
		{
			System.out.println("��ȡ�ļ�����");
			clear();
		}
	}
	//��char[]ת����String
	private String turnToString(char[] buffer, int contentSize)
	{
		String str = "";
		for(int i=0;i<contentSize;i++)
		{
			str += buffer[i];
		}
		return str;
	}
	//��ʽ����ť������
	class formatAction implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			if(JOptionPane.showConfirmDialog(null,"��ѡ�ɾ���ļ�ϵͳ�������ݣ�Ҫ������")
					!=JOptionPane.YES_OPTION)
				return;
			clear();
		}
		
	}


	//��ʽ��
	private void clear()
	{
		UsedSpace = 0;
		FreeSpace = totalSize;
		UsedSpaceDisplay.setText(String.valueOf(UsedSpace));
		FreeSpaceDisplay.setText(String.valueOf(FreeSpace));
		dd.RootFolder = new MyFolder();
		dd.CurrentFolder = dd.RootFolder;
		((DefaultTableModel)dd.DTable.getModel()).getDataVector().removeAllElements();
		((DefaultTableModel)dd.DTable.getModel()).fireTableDataChanged();
	}
	//�����ļ��а�ť������
	class createSubDirAction implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String str = JOptionPane.showInputDialog("������Ҫ�������ļ�������");
			if(str == null)
				return;
			if(str.length()==0)
				JOptionPane.showMessageDialog(null,"�ļ���������Ϊ��");
			else
			{
				if(dd.isSameName(true,str)==false)
					dd.CreateFolder(str);
				else
					JOptionPane.showMessageDialog(null,"��ǰĿ¼���Ѱ���ͬ���ļ���");
			}
		}
		
	}

	//ɾ���ļ��а�ť������
	class deleteSubDirAction implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			int selectedRow = dd.DTable.getSelectedRow();
			if(selectedRow<0 ||
					selectedRow>=dd.CurrentFolder.FolderList.size())
				JOptionPane.showMessageDialog(null,"��ѡ��ֻ����ɾ���ļ���");
			else
			{
				dd.number = 0;
				int size = dd.getFolderSize(dd.CurrentFolder.FolderList.get(selectedRow));
				//System.out.println(size);
				UsedSpace -= size;
				FreeSpace = totalSize-UsedSpace;
				UsedSpaceDisplay.setText(String.valueOf(UsedSpace));
				FreeSpaceDisplay.setText(String.valueOf(FreeSpace));
				dd.DeleteFolder(selectedRow);
			}
		}
		
	}
	
	//�����ļ���ť������
	class createFileAtion implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String str = JOptionPane.showInputDialog("������Ҫ�������ļ�����");
			if(str == null)
				return;
			if(str.length()==0)
				JOptionPane.showMessageDialog(null,"�ļ�������Ϊ��");
			else
			{
				if(dd.isSameName(false,str)==false)
					dd.CreateFile(str);
				else
					JOptionPane.showMessageDialog(null,"��ǰ�ļ������Ѱ���ͬ���ļ�");
			}
		}
		
	}

	//ɾ���ļ���ť������
	class deleteFileAction implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			int selectedRow = dd.DTable.getSelectedRow();
			if((selectedRow-dd.CurrentFolder.FolderList.size())<0 || 
					(selectedRow-dd.CurrentFolder.FolderList.size())>=dd.CurrentFolder.FileList.size())
				JOptionPane.showMessageDialog(null,"��ѡ��ֻ����ɾ���ļ�");
			else
			{
				int size = dd.CurrentFolder.FileList.get(
						selectedRow-dd.CurrentFolder.FolderList.size()).fileSize;
				UsedSpace -= size;
				FreeSpace = totalSize-UsedSpace;
				UsedSpaceDisplay.setText(String.valueOf(UsedSpace));
				FreeSpaceDisplay.setText(String.valueOf(FreeSpace));
				dd.DeleteFile(selectedRow);
			}
		}
		
	}
	//���԰�ť������
	class propertyAction implements ActionListener{

		@Override
			public void actionPerformed(ActionEvent e) 
			{
				int selectedRow = dd.DTable.getSelectedRow();
				if(selectedRow<0 || 
						selectedRow>=dd.CurrentFolder.FolderList.size()
						+dd.CurrentFolder.FileList.size())
					return;
				else if(selectedRow<dd.CurrentFolder.FolderList.size())
					dd.showFolder(selectedRow);
				else
					dd.showFile(selectedRow);
			}		
	}

	//�򿪰�ť������
	class openAction implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub

			int selectedRow = dd.DTable.getSelectedRow();
			if(selectedRow<0 || 
					selectedRow>=dd.CurrentFolder.FolderList.size()
					+dd.CurrentFolder.FileList.size())
				return;
			else if(selectedRow<dd.CurrentFolder.FolderList.size())
			{
				dd.openFolder(selectedRow);
				posLabel.setText(dd.toPosition());
				//createdateLabel
			}
			else
			{
				dd.openFile(selectedRow,UsedSpaceDisplay,FreeSpaceDisplay);
			}
		}
		
	}
	//���˰�ť������
	class backAction implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			dd.backUp();
			posLabel.setText(dd.toPosition());
		}
		
	}

	//ʹ��˵����ť������
	class instructionAction implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String message ="1.��ʽ��: ɾ�������ļ����ļ���\n"
		            +"2.��: �ɴ�ѡ�е��ļ����ļ���\n"
		            +"3.����: ���˵���һĿ¼,�ڸ�Ŀ¼���ɺ���\n"
		            +"4.����: ��ʾ��ѡ�ļ����ļ��еĸ�������\n"
		            +"5.����: ���ļ���������ɽ�����ı䱣�����ļ�\n";
	JOptionPane.showMessageDialog(null, message, "ʹ��˵��", 1);
		}
		
	}

	//��д�˷������û�ȷ���Ƿ���Ҫ�ر�
	protected void processWindowEvent(WindowEvent e)
	{
		boolean flag = false;
		if(e.getID() == WindowEvent.WINDOW_CLOSING)
		{
			//�رյ���ʾѡ��
			int result = JOptionPane.showConfirmDialog(
					this,
					("ȷ��Ҫ�˳���ϵͳ���Զ����浱ǰ����,�����´δ�ʱ�Զ�����."),
					("�˳�"),
					JOptionPane.YES_NO_OPTION);
			if(result == JOptionPane.NO_OPTION)
			{
				flag = true;//���ر�
			}
			else
			{
				//�رյĴ���:������д�����
				try
				{
					saveToText();
				}
				catch (FileNotFoundException e1) 
				{
					e1.printStackTrace();
				}
			}
		}
		if(!flag)
		{
			//�����YES,��ô��������ȥ����رյĴ���
			super.processWindowEvent(e);
		}
	}
	private void saveToText() throws FileNotFoundException
	{
		MyFolder tempFolder = dd.RootFolder;
		Queue <MyFolder> waitingFolders = new ConcurrentLinkedQueue<MyFolder>();
		PrintWriter pw = new PrintWriter(
				new OutputStreamWriter(new FileOutputStream("data.txt")),true);
		waitingFolders.add(tempFolder);
		do
		{
			tempFolder = waitingFolders.peek();
			//д��
			pw.println(tempFolder.FolderList.size());
			for(int i=0;i<tempFolder.FolderList.size();i++)
			{
				pw.println(tempFolder.FolderList.get(i).folderName);
			}
			pw.println(tempFolder.FileList.size());
			for(int i=0;i<tempFolder.FileList.size();i++)
			{
				pw.println(tempFolder.FileList.get(i).fileName);
				pw.println(tempFolder.FileList.get(i).fileSize);
				pw.println(tempFolder.FileList.get(i).fileContent.length());
				pw.println(tempFolder.FileList.get(i).fileContent);
			}

			for(int i=0;i<tempFolder.FolderList.size();i++)
			{
				waitingFolders.add(tempFolder.FolderList.get(i));
			}
			waitingFolders.remove();
		} while (waitingFolders.peek()!=null);

		pw.close();
	}

}

