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
	MyFolder RootFolder;		//根文件夹
	MyFolder CurrentFolder;		//当前打开文件夹
	int number = 0;
	//构造函数
	Display()
	{
		this.setBounds(150, 200, 300, 300);
		DTable = new JTable(0,2)
		{
			private static final long serialVersionUID = -2661555806411459640L;
			public boolean isCellEditable(int row,int col)//内容不可编辑
			{
				return false;
			}
		};
		this.setViewportView(DTable);
		//设置列名
		DTable.getColumnModel().getColumn(0).setHeaderValue("文件名");
		DTable.getColumnModel().getColumn(1).setHeaderValue("大小");
		RootFolder = new MyFolder();
		CurrentFolder = RootFolder;
	}
	//后退
	void backUp()
	{
		if(CurrentFolder == RootFolder)
		{
			JOptionPane.showMessageDialog(null, "已在根目录，无法后退");
			return;
		}
		MyFolder tempFolder = CurrentFolder.fatherFolder;
		CurrentFolder = tempFolder;
		//重置表格
		((DefaultTableModel)DTable.getModel()).getDataVector().removeAllElements();
		int size = CurrentFolder.FolderList.size()+CurrentFolder.FileList.size();
		while(size-->0)
		{
			((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);
		}
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//打开文件夹
	void openFolder(int selectedRow)
	{
		MyFolder tempFolder = CurrentFolder.FolderList.get(selectedRow);
		CurrentFolder = tempFolder;
		//重置表格
		((DefaultTableModel)DTable.getModel()).getDataVector().removeAllElements();
		int size = CurrentFolder.FolderList.size()+CurrentFolder.FileList.size();
		while(size-->0)
		{
			((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);
		}
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//打开文件
	void openFile(int selectedRow, JLabel usedSpaceDisplay, JLabel freeSpaceDisplay)
	{
		MyFile tempFile = CurrentFolder.FileList.get(
				selectedRow-CurrentFolder.FolderList.size());
		FileEditor fe = new FileEditor(tempFile,usedSpaceDisplay,freeSpaceDisplay,this);
		fe.jt.setText(tempFile.fileContent);
		fe.setVisible(true);
		return;
	}
	//显示文件夹属性
	void showFolder(int selectedRow)
	{
		MyFolder tempFolder = CurrentFolder.FolderList.get(selectedRow);
		String str = "";
		str += "文件夹名:         "+tempFolder.folderName;
		str += "\n路径:                "+this.toPosition();
		number = 0;
		str += "\n包含文件夹数:  "+getFolderNumber(tempFolder);
		number = 0;
		str += "\n包含文件数:     "+getFileNumber(tempFolder);
		number = 0;
		str += "\n总大小:            "+getFolderSize(tempFolder)+" Byte";
		str += "\n创建时间：               "+tempFolder.getCreateDate();
		str += "\n修改时间：               "+tempFolder.getChangeDate();
		JOptionPane.showMessageDialog(null,str,"文件夹属性",1);
	}
	//得到当前文件夹包含的文件夹数目
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
	//得到当前文件夹包含的文件数目
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
	//返回当前文件夹大小
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
	//显示文件属性
	void showFile(int selectedRow)
	{
		MyFile tempFile = CurrentFolder.FileList.get(
				selectedRow-CurrentFolder.FolderList.size());
		String str = "";
		str += "文件名:         "+tempFile.fileName;
		str += "\n路径:             "+this.toPosition();
		str += "\n大小:             "+tempFile.fileSize+"  Byte";
		str += "\n创建时间：               "+tempFile.getCreateDate();
		str += "\n修改时间：               "+tempFile.getChangeDate();
		JOptionPane.showMessageDialog(null,str,"文件属性",1);
	}
	//返回当前文件路径
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
	//创建文件
	void CreateFile(String name)
	{
		((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);// 加入空行
		MyFile tempFile = new MyFile(name);
		CurrentFolder.FileList.add(tempFile);
		//System.out.println(tempFile.filePosition);
		updateTable();
	}
	//删除文件
	void DeleteFile(int selectedRow)
	{
		CurrentFolder.FileList.remove(selectedRow-CurrentFolder.FolderList.size());
		((DefaultTableModel)DTable.getModel()).getDataVector().remove(selectedRow);
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//创建文件夹
	void CreateFolder(String folderName)
	{
		((DefaultTableModel)DTable.getModel()).addRow(new Object[2]);// 加入空行
		MyFolder tempFolder = new MyFolder(CurrentFolder,folderName);
		CurrentFolder.FolderList.add(tempFolder);
		updateTable();
	}
	//删除文件夹
	void DeleteFolder(int selectedRow)
	{
		CurrentFolder.FolderList.remove(selectedRow);
		((DefaultTableModel)DTable.getModel()).getDataVector().remove(selectedRow);//删除当前行
		((DefaultTableModel)DTable.getModel()).fireTableDataChanged();
		updateTable();
	}
	//判断是否重名
	boolean isSameName(boolean folder,String str)
	{
		if(folder)	//文件夹
		{
			for(int i=0;i<CurrentFolder.FolderList.size();i++)
			{
				if(str.equals(CurrentFolder.FolderList.get(i).folderName))
					return true;
			}
		}
		else		//文件
		{
			for(int i=0;i<CurrentFolder.FileList.size();i++)
			{
				if(str.equals(CurrentFolder.FileList.get(i).fileName))
					return true;
			}
		}
		return false;
	}
	//更新列表
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
//文本编辑窗口
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
		super(file.fileName+" - 文件编辑");
		setLayout(null);//禁用布局
		
		this.setResizable(false);//大小不可调整
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
		save = new JButton("保存");
		save.setBounds(40, 215, 80, 30);
		save.setFont(new Font("保存", Font.HANGING_BASELINE, 14));
		addSaveListenser(save);
		add(save);
		cancel = new JButton("取消");
		cancel.setBounds(160, 215, 80, 30);
		cancel.setFont(new Font("取消", Font.HANGING_BASELINE, 14));
		addCancelListenser(cancel);
		add(cancel);
	}
	//保存按钮监听器
	void addSaveListenser(JButton button)
	{
		ActionListener aL = new ActionListener() 
		{
			public void actionPerformed(ActionEvent e) 
			{
				String str = jt.getText();
				if(str.length()*2>MainFrame.FreeSpace)
				{
					JOptionPane.showMessageDialog(null, "系统空间不足");
					return;
				}
				save(str);
			}
		};
		button.addActionListener(aL);
	}
	//取消按钮监听器
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
	//保存当前输入到文件
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
	//得到当前文件大小
	int getFileSize()
	{
		int size = currentFile.fileSize;
		return size;
	}
	//更新显示空间
	void updateSpaceDisplay(int oldSize,int newSize)
	{
		MainFrame.UsedSpace -= oldSize;
		MainFrame.UsedSpace += newSize;
		MainFrame.FreeSpace = 1024-MainFrame.UsedSpace;
		usedSpaceDisplay.setText(String.valueOf(MainFrame.UsedSpace));
		freeSpaceDisplay.setText(String.valueOf(MainFrame.FreeSpace));
	}
}

