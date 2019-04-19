Name:           nem-nem
Version:        2011.05.1b
Summary:        A dice game like Yam's or Yahtzee
Release:        %mkrel 1
License:        GPLv3
Group:          Games/Boards
URL:            http://code.google.com/p/nem-nem/downloads/list
Source0:        nem-nem-2011.05.1b.tar.gz

BuildRoot:      %_tmppath/%name-%version-%release-buildroot
BuildRequires:  qt4-devel >= 4.6
BuildRequires:  sox
Requires:		qt4-common >= 4.6
Requires:		sox

%description
A dice game like Yam's or Yahtzee

%files
%defattr(-,root,root)
%_gamesdatadir/nem-nem
%_gamesbindir/nem-nem
%_datadir/applications/nem-nem.desktop
%_iconsdir/nem-nem.png

#-----------------------------------------------------------------------------

%prep
%setup -q -n nem-nem

%build

%qmake_qt4 nem-nem.pro

%make


%install
rm -rf %buildroot
%__mkdir -p  %buildroot%_gamesdatadir/nem-nem
%__cp nem-nem %buildroot%_gamesdatadir/nem-nem/
%__cp *.qm  %buildroot%_gamesdatadir/nem-nem/

%__mkdir -p %buildroot%_gamesdatadir/nem-nem/images
%__cp images/* %buildroot%_gamesdatadir/nem-nem/images

%__mkdir -p %buildroot%_gamesbindir
ln -s %_gamesdatadir/nem-nem/nem-nem %buildroot%_gamesbindir

%__mkdir -p %buildroot%_datadir/applications
%__cp desktop/nem-nem.desktop %buildroot%_datadir/applications/nem-nem.desktop

%__mkdir -p %buildroot%_iconsdir
%__cp desktop/nem-nem.png %buildroot%_iconsdir/nem-nem.png

%clean
rm -rf %{buildroot}



%changelog
* Wed May 25 2011 Gilles Pascual <gpascual@mandriva.org> 2011.05.1b-1mdv2010.2
- New human readable save format

* Sun Mar 27 2011 Gilles Pascual <gpascual@mandriva.org> 2011.03.4-1mdv2010.1
- SPEC update

* Mon Mar 21 2011 Gilles Pascual <gpascual@mandriva.org> 2011.03.2-1mdv2010.1
- SPEC update

* Sat Jul 03 2010 Gilles Pascual <gpascual@mandriva.org> 2010.07.1-1mdv2010.1
- Bug fix on undo management
- Improve Robot play

* Sat Jun 05 2010 Gilles Pascual <gpascual@mandriva.org> 2010.06.1-1mdv2010.1
- No more row CHANCE, replace by 2 new rows. 1 new column, demo mode.

* Wed Mar 24 2010 Gilles Pascual <gpascual@mandriva.org> 2010.03-1mdv2010.1
- Sounds in Nem-Nem

* Sun Feb 07 2010 Gilles Pascual <gpascual@mandriva.org> 2010.01.4-1mdv2010.1
- Some display optimizations

* Thu Jan 28 2010 Gilles Pascual <papajaac@gmail.com> 2010.01.3-1mdv2010.1
- New menu action + ESC key to return from help page, preferences... to main page

* Thu Jan 28 2010 Stephane Téletchéa <steletch@mandriva.org> 2010.01.1-2mdv2010.1
- Spec corrections and updates

* Wed Jan 27 2010 Gilles Pascual <gpascual@laposte.net> 2010.01.1-1mdv2010.1
- Initial Mandriva package


