---
title: "Remote editing/development"
katex: false
weight: 5
---

# Remote development and managing changes

## Remote editing

Editing in a terminal is somewhat painful, and laboriously copying
files back and forth between your local machine and Hamilton is
error-prone.

A better approach is to use a combination of version control and
remote editing. I provide brief instructions for [VS
Code](https://code.visualstudio.com) which is a popular editor, and
very brief instructions for Emacs. 

### VS Code

VS Code has an extension for [remote
editing](https://code.visualstudio.com/docs/remote/remote-overview)
which allows you to edit files on remote systems as if you're running
locally.

Search the extensions marketplace and install the [Remote
Development](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.vscode-remote-extensionpack)
extension pack. You can then connect to a remote SSH server (of which
Hamilton is an example). If you [followed the instructions]({{< ref
"configuration.md" >}}) to setup simpler ssh logins, then just hit
`F1` (to pop up the "command palette") and run the "Remote-SSH: Connect
to Host" command.

You can then use VS code, browsing the Hamilton filesystem as if it
were local.

{{< hint info >}}

I don't use VS Code myself, so can't help with further configuration
or editing, but hopefully this is useful.

{{< /hint >}}

### Emacs

If you use Emacs, then
[TRAMP](https://www.emacswiki.org/emacs/TrampMode) offers similar
functionality, you can browse the remote filesystem and edit as normal.

If you want to use [magit](https://magit.vc), the version of `git`
available on Hamilton is unfortunately too old. You might be able to
compile your own version and put it in your `PATH`, you'll then need
to add `(add-to-list 'tramp-remote-path 'tramp-own-remote-path)` to
your `.emacs`.


## Managing changes with version control

All of the material for the course is maintained in a git repository
on [GitHub]({{< repo >}}). This makes it easy for me to keep things up
to date. If you have followed the advice to clone a copy of the
repository then you probably want to know how to

1. Keep getting updates that I create
2. Make sure your changes are saved
3. Move your changes between computers (for example off Hamilton)

To do this, I recommend the following workflow. You will need a
[GitHub account](https://github.com), so if you don't have one, go
away and register.

{{< hint info >}}

The free tier of GitHub disables some useful features. Fortunately,
as a student, you can sign up to an educational license which upgrades
your account for free. After creating your github account, sign up for
the [student developer pack](https://education.github.com/pack). Your
Durham email will need to be associated with your account, follow
instructions
[here](https://docs.github.com/en/enterprise/2.15/user/articles/adding-an-email-address-to-your-github-account)
if it isn't already.

{{< /hint >}}

With your new account available, "fork" the course repository. This
creates a copy associated with your account which you have edit access
in.

Now you can edit and commit away in the repository and push and pull
any changes to your fork.

To synchronise with my version, you can either do it [on the
commandline](https://docs.github.com/en/free-pro-team@latest/github/collaborating-with-issues-and-pull-requests/syncing-a-fork),
or in the GitHub web interface with pull requests. The [Earth
Lab](https://www.earthdatascience.org) have a [nice
tutorial](https://www.earthdatascience.org/courses/intro-to-earth-data-science/git-github/github-collaboration/update-github-repositories-with-changes-by-others/)
for the latter approach.
