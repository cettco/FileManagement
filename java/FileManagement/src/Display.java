import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextPane;
import javax.swing.table.DefaultTableModel;

class Display extends JScrollPane
{
	private static final long serialVersionUID = 4400443870725369626L;
	JTable DTable;
	MyFolder RootFolder;		//���ļ���
	MyFolder CurrentFolder;		//��ǰ���ļ���
	int number = 0;
	//���캯��
	Display()
	{
		this.setBounds(150, 200, 300, 300);
		DTable = new JTable(0,2)
		{
			private static final long serialVersionUID = -2661555806411459640L;
			public boolean isCellEditable(int row,int col)//���ݲ��ɱ༭
			{
				return false;
			}
		};
		this.setViewportView(DTable);
		//��������
		DTable.getColumnModel().getColumn(0).setHeaderValue("�ļ���");
		DTable.getColumnModel().getColumn(1).setHeaderValue("��С");
		RootFolder = new MyFolder();
		CurrentFolder = RootFolder;
	}
	//����
	void backUp()
	{
		if(CurrentFolder == RootFolder)
		{
			JOptionPane.showMessageDialog(null, "���ڸ�Ŀ¼���޷�����");
			return;
		}
		MyFolder tempFolder = CurrentFolder.fatherFolder;
		CurrentFolder = tempFolder;
		//���ñ��
		((DefaultTableModel)DTable.getModel()).getDataVector().removeAllElements();
		int size = CurrentFolder.FolderList.size()+CurrentFolder.FileList.size();
		while(size-->0)
		{
			((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);
		}
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//���ļ���
	void openFolder(int selectedRow)
	{
		MyFolder tempFolder = CurrentFolder.FolderList.get(selectedRow);
		CurrentFolder = tempFolder;
		//���ñ��
		((DefaultTableModel)DTable.getModel()).getDataVector().removeAllElements();
		int size = CurrentFolder.FolderList.size()+CurrentFolder.FileList.size();
		while(size-->0)
		{
			((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);
		}
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//���ļ�
	void openFile(int selectedRow, JLabel usedSpaceDisplay, JLabel freeSpaceDisplay)
	{
		MyFile tempFile = CurrentFolder.FileList.get(
				selectedRow-CurrentFolder.FolderList.size());
		FileEditor fe = new FileEditor(tempFile,usedSpaceDisplay,freeSpaceDisplay,this);
		fe.jt.setText(tempFile.fileContent);
		fe.setVisible(true);
		return;
	}
	//��ʾ�ļ�������
	void showFolder(int selectedRow)
	{
		MyFolder tempFolder = CurrentFolder.FolderList.get(selectedRow);
		String str = "";
		str += "�ļ�����:         "+tempFolder.folderName;
		str += "\n·��:                "+this.toPosition();
		number = 0;
		str += "\n�����ļ�����:  "+getFolderNumber(tempFolder);
		number = 0;
		str += "\n�����ļ���:     "+getFileNumber(tempFolder);
		number = 0;
		str += "\n�ܴ�С:            "+getFolderSize(tempFolder)+" Byte";
		str += "\n����ʱ�䣺               "+tempFolder.getCreateDate();
		str += "\n�޸�ʱ�䣺               "+tempFolder.getChangeDate();
		JOptionPane.showMessageDialog(null,str,"�ļ�������",1);
	}
	//�õ���ǰ�ļ��а������ļ�����Ŀ
	int getFolderNumber(MyFolder folder)
	{
		if(folder.FolderList.size()>0)
		{
			for(int i=0;i<folder.FolderList.size();i++)
			{
				getFolderNumber(folder.FolderList.get(i));
				number++;
			}
		}
		return number;
	}
	//�õ���ǰ�ļ��а������ļ���Ŀ
	int getFileNumber(MyFolder folder)
	{
		if(folder.FolderList.size()>0)
		{
			for(int i=0;i<folder.FolderList.size();i++)
			{
				getFileNumber(folder.FolderList.get(i));
			}
		}
		if(folder.FileList.size()>0)
		{
			number += folder.FileList.size();
		}
		return number;
	}
	//���ص�ǰ�ļ��д�С
	int getFolderSize(MyFolder folder)
	{
		if(folder.FolderList.size()>0 || folder.FileList.size()>0)
		{
			for(int i=0;i<folder.FolderList.size();i++)
			{
				getFolderSize(folder.FolderList.get(i));
			}
			for(int j=0;j<folder.FileList.size();j++)
			{
				number += folder.FileList.get(j).fileSize;
			}
		}
		return number;
	}
	//��ʾ�ļ�����
	void showFile(int selectedRow)
	{
		MyFile tempFile = CurrentFolder.FileList.get(
				selectedRow-CurrentFolder.FolderList.size());
		String str = "";
		str += "�ļ���:         "+tempFile.fileName;
		str += "\n·��:             "+this.toPosition();
		str += "\n��С:             "+tempFile.fileSize+"  Byte";
		str += "\n����ʱ�䣺               "+tempFile.getCreateDate();
		str += "\n�޸�ʱ�䣺               "+tempFile.getChangeDate();
		JOptionPane.showMessageDialog(null,str,"�ļ�����",1);
	}
	//���ص�ǰ�ļ�·��
	public String toPosition() 
	{
		MyFolder tempFolder = CurrentFolder;
		String str = "";
		do
		{
			str=tempFolder.folderName+"\\"+str;
			tempFolder = tempFolder.fatherFolder;
		}while(tempFolder!=null);
		return str;
	}
	//�����ļ�
	void CreateFile(String name)
	{
		((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);// �������
		MyFile tempFile = new MyFile(name);
		CurrentFolder.FileList.add(tempFile);
		//System.out.println(tempFile.filePosition);
		updateTable();
	}
	//ɾ���ļ�
	void DeleteFile(int selectedRow)
	{
		CurrentFolder.FileList.remove(selectedRow-CurrentFolder.FolderList.size());
		((DefaultTableModel)DTable.getModel()).getDataVector().remove(selectedRow);
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//�����ļ���
	void CreateFolder(String folderName)
	{
		((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);// �������
		MyFolder tempFolder = new MyFolder(CurrentFolder,folderName);
		CurrentFolder.FolderList.add(tempFolder);
		updateTable();
	}
	//ɾ���ļ���
	void DeleteFolder(int selectedRow)
	{
		CurrentFolder.FolderList.remove(selectedRow);
		((DefaultTableModel)DTable.getModel()).getDataVector().remove(selectedRow);//ɾ����ǰ��
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//�ж��Ƿ�����
	boolean isSameName(boolean folder,String str)
	{
		if(folder)	//�ļ���
		{
			for(int i=0;i<CurrentFolder.FolderList.size();i++)
			{
				if(str.equals(CurrentFolder.FolderList.get(i).folderName))
					return true;
			}
		}
		else		//�ļ�
		{
			for(int i=0;i<CurrentFolder.FileList.size();i++)
			{
				if(str.equals(CurrentFolder.FileList.get(i).fileName))
					return true;
			}
		}
		return false;
	}
	//�����б�
	void updateTable()
	{
		int row = 0;
		for(int i=0;i<CurrentFolder.FolderList.size();i++)
		{
			String str = "["+CurrentFolder.FolderList.get(i).folderName+"]";
			DTable.setValueAt(str, row, 0);
			DTable.setValueAt("  ", row++, 1);
		}
		for(int i=0;i<CurrentFolder.FileList.size();i++)
		{
			DTable.setValueAt(CurrentFolder.FileList.get(i).fileName, row, 0);
			DTable.setValueAt(CurrentFolder.FileList.get(i).fileSize+"  Byte", row++, 1);
		}
		DTable.updateUI();
	}
}
//�ı��༭����
class FileEditor extends JFrame
{
	private static final long serialVersionUID = 4314644824837823245L;
	JScrollPane js;
	JTextPane jt;
	JButton save;
	JButton cancel;
	MyFile currentFile;
	JLabel usedSpaceDisplay;
	JLabel freeSpaceDisplay;
	Display directoryDisplay;
	FileEditor(MyFile file, JLabel usedSpaceDisplay, JLabel freeSpaceDisplay, Display dd)
	{
		super(file.fileName+" - �ļ��༭");
		setLayout(null);//���ò���
		
		this.setResizable(false);//��С���ɵ���
		this.setBounds(200, 200, 300, 300);
		currentFile = file;
		this.usedSpaceDisplay = usedSpaceDisplay;
		this.freeSpaceDisplay = freeSpaceDisplay;
		directoryDisplay = dd;

		js = new JScrollPane();
		js.setBounds(0, 0, 285, 200);
		jt = new JTextPane();
		js.setViewportView(jt);
		add(js);
		save = new JButton("����");
		save.setBounds(40, 215, 80, 30);
		save.setFont(new Font("����", Font.HANGING_BASELINE, 14));
		addSaveListenser(save);
		add(save);
		cancel = new JButton("ȡ��");
		cancel.setBounds(160, 215, 80, 30);
		cancel.setFont(new Font("ȡ��", Font.HANGING_BASELINE, 14));
		addCancelListenser(cancel);
		add(cancel);
	}
	//���水ť������
	void addSaveListenser(JButton button)
	{
		ActionListener aL = new ActionListener() 
		{
			public void actionPerformed(ActionEvent e) 
			{
				String str = jt.getText();
				if(str.length()*2>MainFrame.FreeSpace)
				{
					JOptionPane.showMessageDialog(null, "ϵͳ�ռ䲻��");
					return;
				}
				save(str);
			}
		};
		button.addActionListener(aL);
	}
	//ȡ����ť������
	void addCancelListenser(JButton button)
	{
		ActionListener aL = new ActionListener() 
		{
			public void actionPerformed(ActionEvent e) 
			{
				dispose();
			}
		};
		button.addActionListener(aL);
	}
	//���浱ǰ���뵽�ļ�
	void save(String str)
	{
		int oldSize = getFileSize();
		currentFile.fileContent = str;
		currentFile.setDate();
		currentFile.update();
		int newSize = getFileSize();
		//System.out.println(oldSize+" , "+newSize);
		directoryDisplay.updateTable();
		updateSpaceDisplay(oldSize,newSize);
	}
	//�õ���ǰ�ļ���С
	int getFileSize()
	{
		int size = currentFile.fileSize;
		return size;
	}
	//������ʾ�ռ�
	void updateSpaceDisplay(int oldSize,int newSize)
	{
		MainFrame.UsedSpace -= oldSize;
		MainFrame.UsedSpace += newSize;
		MainFrame.FreeSpace = 1024-MainFrame.UsedSpace;
		usedSpaceDisplay.setText(String.valueOf(MainFrame.UsedSpace));
		freeSpaceDisplay.setText(String.valueOf(MainFrame.FreeSpace));
	}
}

