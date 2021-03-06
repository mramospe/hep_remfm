.. _file-management:

File management
---------------

In section :ref:`setup` we saw how to set-up two tables in two different hosts,
one local and one remote, with different files.
Our final goal is to **access these files from python**.
In the local host we can do it by writing

.. code-block:: python

   >>> import hep_rfm
   >>> table = hep_rfm.Table.read('/home/user/rfm/table.db')
   >>> list(sorted(table.keys()))
   ['file1', 'file2', 'file3', 'file4', 'file5', 'file6']
   >>> table['file1']
   FileInfo(name='file1', protocol_path=LocalPath(path='/home/user/rfm/files/file1.txt'), marks=FileMarks(tmstp=1540986365.8954208, fid='202444300d48c9387406493590cc693a81c011f1c112d7c8916b27ff9e5daad32a679e4471974b2169c4d22738a17a92405d8948bf5b1d2d8b69c235a65f991b'))
   >>> table['file1'].protocol_path.path
   /home/user/rfm/files/file1.txt

so we can access all the information.
We could also modify the information of the table, make copies, and then
write them using the method :meth:`hep_rfm.Table.write`.
To automatically update the table using the local files, we could
simply call :meth:`hep_rfm.Table.updated`, and we would obtain
the updated version of the table.

From now on, we will consider that we are working in the local host.
To manage both tables, we will create a :class:`hep_rfm.Manager` class, and
register the two tables

.. code-block:: python

   >>> mgr = hep_rfm.Manager()
   >>> mgr.add_table('/home/user/rfm/table.db', 'local')
   >>> mgr.add_table('@host.com:/home/user/table.db', 'ssh')

Now we have the two tables registered.
Calling the method :meth:`hep_rfm.Manager.available_table`, we will
obtain the table corresponding to the local path.
This means that in the local host we will get "/home/user/rfm/table.db",
and in the remote "/home/user/table.db".
With this information we can create the table, and work with the files
from there.

.. code-block:: python

   >>> table = mgr.available_table()
   >>> list(sorted(table.keys()))
   ['file1', 'file2', 'file3', 'file4', 'file5', 'file6']
   >>> hep_rfm.available_working_path(table['file1'])
   /home/user/rfm/files/file1.txt

The advantage of using the :class:`hep_rfm.Manager` instance, is that
we do not need to care whether we are in the local or remote host.
This turns powerful when having code on a `git <https://git-scm.com/>`_
repository, which is present in both local and remote hosts, since one
could work in each host without really caring about the location of the
files.
Lastly, and probably one of the most useful methods of the class
concerns the method :class:`hep_rfm.Manager.update`.
This method allows to update all the different tables stored in the manager.

1. First, the **tables are copied** to the current host.
2. On a second step, the **file IDs** of the files **are compared** among the different
   tables.
3. If there is a mismatch, then the **time-stamps are checked**, so the newest
   version is determined.
4. After, the **files are copied** to each outdated location.
5. Finally, the **tables** in the outdated locations **are updated**.

The general idea is to have a python file in your module with the tables
begin defined, so it serves as an access point to the data and also to
update it.
An example of such module would be

.. code-block:: python

   mgr = hep_rfm.Manager()
   mgr.add_table('/home/user/rfm/table.db', 'local')
   mgr.add_table('@host.com:/home/user/table.db', 'ssh')

   table = mgr.available_table()

   if __name__ == '__main__':
       mgr.update(modifiers={'ssh_usernames': {'host.com': 'username'}})

so if we put this in a file called "data.py", we can access it via
:code:`import data` and then :code:`data.table` would allow us to
access the real paths of the files.
On the other hand, the samples would be updated executing :code:`python data.py`.
Note that we have specified the keyword argument *modifiers*.
This allows us to specify in a dictionary the user-name to be used for each ssh
host, for example.
