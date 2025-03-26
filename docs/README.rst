:orphan:

How to update this manual
=========================

How to install the documentation toolchain
------------------------------------------

#. Install Python, Doxygen and Graphwiz (dot)::

    sudo apt install python3 doxygen graphviz

#. Go to the M-Bus source directory::

    cd m-bus

#. Create a `virtual environment <https://docs.python.org/3/tutorial/venv.html>`_::

    python3 -m venv .venv

#. Activate the virtual environment::

    source .venv/bin/activate

#. Install Sphinx and other needed Python packages::

    pip3 install -r docs/requirements.txt


How to build the manual
-----------------------

#. Install the documentation toolchain if not already done (see above).

#. Activate the virtual environment if not already done so (see above)::

    source .venv/bin/activate

#. Create the build system (``-DCMAKE_IGNORE_PATH:PATH=$HOME/.local/bin``
   prevents cmake from using a script installed elsewhere)::

    cmake --preset docs -DCMAKE_IGNORE_PATH:PATH=$HOME/.local/bin

#. Build the manual as HTML::

    cmake --build --preset docs

#. View the the generated manual at :file:`build/docs/docs/sphinx/html/index.html`.


How to write documentation
--------------------------

Documentation is written in reStructuredText (reST). For details on the syntax,
see https://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html

The guideline followed is https://diataxis.fr/, where documentation is
divided into *Explanation*, *Tutorials*, *How-to guides*
and *Reference*.

New files are placed in the folder of their corresponding chapter and are added
to its :file:`index.rst` file::

  * :file:`doc/introduction/index.rst` for *Explanation*
  * :file:`doc/tutorials/index.rst` for *Tutorials*
  * :file:`doc/how-to-guides/index.rst` for *How-to guides*
  * :file:`doc/reference-manual/index.rst` for *Reference*


How to create figures
---------------------

Figures are generated using the kroki plugin. See https://kroki.io/examples.html.

Examples:

.. kroki:: svg
   :type: blockdiag
   :caption: Block diagram

   blockdiag {
      orientation = portrait

      A -> B -> C;
      B -> D;
   }

.. kroki:: svg
   :type: seqdiag
   :caption: Sequence diagram

   seqdiag {
      browser  -> webserver [label = "GET /index.html"];
      browser <-- webserver;
      browser  -> webserver [label = "POST /blog/comment"];
      webserver  -> database [label = "INSERT comment"];
      webserver <-- database;
      browser <-- webserver;
   }

.. kroki:: svg
   :type: plantuml
   :caption: State machine

   @startuml
   state A {
      state X {
      }
      state Y {
      }
   }

   state B {
      state Z {
      }
   }

   X --> Z
   Z --> Y
   @enduml

.. kroki::
   :type: packetdiag
   :caption: TCP packet

   packetdiag {
      colwidth = 32;
      node_height = 72;

      0-15: Source Port;
      16-31: Destination Port;
      32-63: Sequence Number;
      64-95: Acknowledgment Number;
      96-99: Data Offset;
      100-105: Reserved;
      106: URG [rotate = 270];
      107: ACK [rotate = 270];
      108: PSH [rotate = 270];
      109: RST [rotate = 270];
      110: SYN [rotate = 270];
      111: FIN [rotate = 270];
      112-127: Window;
      128-143: Checksum;
      144-159: Urgent Pointer;
      160-191: (Options and Padding);
      192-223: data [colheight = 3];
   }
