#include "sshsocket.h"
#include <QFileInfo>
// if compiling in windows, add needed flags.
/*#ifdef _WIN32
#   include <io.h>

typedef int mode_t;

/// @Note If STRICT_UGO_PERMISSIONS is not defined, then setting Read for any
///       of User, Group, or Other will set Read for User and setting Write
///       will set Write for User.  Otherwise, Read and Write for Group and
///       Other are ignored.
///
/// @Note For the POSIX modes that do not have a Windows equivalent, the modes
///       defined here use the POSIX values left shifted 16 bits.

static const mode_t S_ISUID      = 0x08000000;           ///< does nothing
static const mode_t S_ISGID      = 0x04000000;           ///< does nothing
static const mode_t S_ISVTX      = 0x02000000;           ///< does nothing
static const mode_t S_IRUSR      = mode_t(_S_IREAD);     ///< read by user
static const mode_t S_IWUSR      = mode_t(_S_IWRITE);    ///< write by user
static const mode_t S_IXUSR      = 0x00400000;           ///< does nothing
#   ifndef STRICT_UGO_PERMISSIONS
static const mode_t S_IRGRP      = mode_t(_S_IREAD);     ///< read by *USER*
static const mode_t S_IWGRP      = mode_t(_S_IWRITE);    ///< write by *USER*
static const mode_t S_IXGRP      = 0x00080000;           ///< does nothing
static const mode_t S_IROTH      = mode_t(_S_IREAD);     ///< read by *USER*
static const mode_t S_IWOTH      = mode_t(_S_IWRITE);    ///< write by *USER*
static const mode_t S_IXOTH      = 0x00010000;           ///< does nothing
#   else
static const mode_t S_IRGRP      = 0x00200000;           ///< does nothing
static const mode_t S_IWGRP      = 0x00100000;           ///< does nothing
static const mode_t S_IXGRP      = 0x00080000;           ///< does nothing
static const mode_t S_IROTH      = 0x00040000;           ///< does nothing
static const mode_t S_IWOTH      = 0x00020000;           ///< does nothing
static const mode_t S_IXOTH      = 0x00010000;           ///< does nothing
#   endif
static const mode_t MS_MODE_MASK = 0x0000ffff;           ///< low word
#endif

*/
SSHSocket::SSHSocket(QObject * parent )
    :QThread(parent)
{
    m_host = "";
    m_user = "";
    m_password = "";
    m_port = 22;
    m_authenticated = false;
    m_session  = NULL;
    m_workingDirectory = ".";

    qRegisterMetaType<SSHSocket::SSHSocketError>("SSHSocket::SSHSocketError");

    start();
    m_run = true;
}

SSHSocket::~SSHSocket()
{
    m_run = false;
    this->wait();
}

void SSHSocket::run()
{

    while(m_run)
    {
        if (m_session == NULL)
        {
            if (m_host != "")
            {

                m_session = ssh_new();

                //set logging to verbose so all errors can be debugged if crash happens
                int verbosity = SSH_LOG_PROTOCOL;

                // set the pertinant ssh session options
                ssh_options_set(m_session, SSH_OPTIONS_HOST, m_host.toAscii().data());
                ssh_options_set(m_session, SSH_OPTIONS_USER, m_user.toAscii().data());
                ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
                ssh_options_set(m_session, SSH_OPTIONS_PORT, &m_port);

                // try to connect given host, user, port
                int connectionResponse = ssh_connect(m_session);

                // if connection is successfull keep track of connection info.
                if (connectionResponse == SSH_OK)
                    connected();
                else
                    m_emitError(SessionCreationError);
            }

        }


        // if we have a vaild ssh connection, authenticate connection with credentials
        else if (!m_authenticated)
        {


            // check to see if a username and a password have been given
            if (m_user != "" && m_password !="")
            {

                // try authenticating current user at remote host
                int worked = ssh_userauth_password(m_session, m_user.toAscii().data(), m_password.toAscii().data());


                // if successful, store user password.
                if (worked == SSH_OK)
                {
                    authenticated();
                    m_authenticated = true;
                }
                else
                {
                    m_user = "";
                    m_password = "";
                    m_emitError(PasswordAuthenticationFailedError);
                }


            }
        }
        // if all ssh setup has been completed, check to see if we have any commands to execute
        else if (m_operationsToExecute.count() > 0)
        {
            SSHOperation operation = m_operationsToExecute[0];
            m_operationsToExecute.pop_front();

            if (operation.type == Command || operation.type == WorkingDirectoryTest)
            {

                // attempt to open ssh shell channel
                ssh_channel channel = ssh_channel_new(m_session);

                // if attempt fails,return
                if (ssh_channel_open_session(channel) != SSH_OK)
                {
                    m_emitError(ChannelCreationError);
                }


                int requestResponse = SSH_AGAIN;

                // attempt to execute shell command
                while (requestResponse == SSH_AGAIN)
                    requestResponse = ssh_channel_request_exec(channel, operation.adminCommand.toAscii().data());

                // if attempt not executed, close connection then return
                if (requestResponse != SSH_OK)
                {
                    m_emitError(ChannelCreationError);
                }


                QByteArray buffer;
                buffer.resize(1000);

                // read in command result
                int totalBytes = 0, newBytes = 0;
                do
                {
                    newBytes = ssh_channel_read(channel, &(buffer.data()[totalBytes]), buffer.size() - totalBytes, 0);
                    if (newBytes > 0)
                        totalBytes += newBytes;
                }while (newBytes > 0);

                // close channel
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
                ssh_channel_free(channel);

                QString response = QString(buffer).mid(0,totalBytes);
                response.replace("\n","");
                if (operation.type == WorkingDirectoryTest)
                {
                    if (response == "exists")
                        m_workingDirectory = m_nextWorkingDir;
                    m_nextWorkingDir = ".";
                    workingDirectorySet(m_workingDirectory);
                }
                else
                    commandExecuted( operation.command, response) ;

            }
            // if all ssh setup has been completed, check to see if we have any file transfers to execute
            else if (operation.type == Pull)
            {
                ssh_scp scpSession = ssh_scp_new(m_session,SSH_SCP_READ, operation.remotePath.toAscii().data());
                if (scpSession == NULL)
                    m_emitError(SCPChannelCreationError);

                // attempt to initialize new scp session.
                int scpInitialized = ssh_scp_init(scpSession);
                if(scpInitialized != SSH_OK)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SCPChannelCreationError);
                }


                // attempt to authorize new scp pull
                if (ssh_scp_pull_request(scpSession) != SSH_SCP_REQUEST_NEWFILE)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SCPPullRequestError);
                }

                // accept authorization
                ssh_scp_accept_request(scpSession);


                // get remote file size
                int size = ssh_scp_request_get_size(scpSession);

                // resize buffer, read remote file into buffer
                QByteArray buffer;
                buffer.resize(size);

                // if an error happens while reading, close the scp session and return
                if (ssh_scp_read(scpSession, buffer.data() , size) == SSH_ERROR)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SocketError);
                }

                // loop until eof flag
                if  (ssh_scp_pull_request(scpSession)  != SSH_SCP_REQUEST_EOF)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SocketError);
                }

                //close scp session
                ssh_scp_close(scpSession);
                ssh_scp_free(scpSession);

                // open up local file and write contents of buffer to it.
                QFile file(operation.localPath);
                file.open(QIODevice::WriteOnly);
                file.write(buffer);
                file.close();

                pullSuccessfull(operation.localPath,operation.remotePath);

            }
            else if (operation.type == Push)
            {
                // attempt to create new scp from ssh session.
                ssh_scp scpSession = ssh_scp_new(m_session,SSH_SCP_WRITE, operation.remotePath.toAscii().data());

                // if creation failed, return
                if (scpSession == NULL)
                    m_emitError(SocketError);


                // attempt to initialize new scp session.
                int scpInitialized = ssh_scp_init(scpSession);


                // if failed, close scp session and return.
                if(scpInitialized != SSH_OK)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SCPChannelCreationError);
                }


                // open the local file and check to make sure it exists
                // if not, close scp session and return.
                QFile file(operation.localPath);
                if (!file.exists())
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SCPFileNotCreatedError);
                }

                // if the file does exist, read all contents as bytes
                file.open(QIODevice::ReadOnly);
                QByteArray buffer =file.readAll();
                file.close();

                // attempt to authorize pushing bytes over scp socket
                // if this fails, close scp session and return.
                if (ssh_scp_push_file(scpSession, operation.remotePath.toAscii().data(), buffer.size(), S_IRUSR | S_IWUSR) != SSH_OK)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SCPPushRequestError);
                }


                // once authorized to push bytes over scp socket, start writing
                // if an error is returned,  close scp session and return.
                if ( ssh_scp_write(scpSession,buffer.data(), buffer.size()) != SSH_OK)
                {

                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    m_emitError(SCPWriteError);
                }


                // close scp session and return.
                ssh_scp_close(scpSession);
                ssh_scp_free(scpSession);

                pushSuccessfull(operation.localPath,operation.remotePath);

            }


        }
        else
        {
            msleep(100);
        }

    }

}
void SSHSocket::disconnectFromHost()
{
    m_host = "";
    m_user = "";
    m_password = "";
    m_port = -1;
    m_authenticated = false;
    m_operationsToExecute.clear();
    if (m_session != NULL)
    {
        // disconnect from current session
        ssh_disconnect(m_session);
        ssh_free(m_session);
    }
    m_session = NULL;
}

void SSHSocket::connectToHost(QString host, int port)
{

    m_host = host;
    m_port = port;

}
void SSHSocket::login(QString user, QString password)
{
    m_user = user;
    m_password = password;
}
void SSHSocket::executeCommand(QString command)
{
    SSHOperation commandOp;
    commandOp.type = Command;
    if (m_workingDirectory != ".")
        commandOp.adminCommand = "cd " + m_workingDirectory + "; "  + command;
    else
        commandOp.adminCommand = command ;

    commandOp.command =command;
    m_operationsToExecute.append(commandOp);
}

void SSHSocket::pullFile(QString localPath, QString remotePath)
{
    SSHOperation transferOp;
    transferOp.localPath = localPath;
    if (QFileInfo(remotePath).isAbsolute())
        transferOp.remotePath = remotePath;
    else
        transferOp.remotePath = m_workingDirectory + "/" + remotePath;
    transferOp.type = Pull;
    m_operationsToExecute.append(transferOp);
}

void SSHSocket::pushFile(QString localPath, QString remotePath)
{

    SSHOperation transferOp;
    transferOp.localPath = localPath;
    if (QFileInfo(remotePath).isAbsolute())
        transferOp.remotePath = remotePath;
    else
        transferOp.remotePath = m_workingDirectory + "/" + remotePath;
    transferOp.type = Push;
    m_operationsToExecute.append(transferOp);
}

void SSHSocket::setWorkingDirectory(QString path)
{
    m_nextWorkingDir = path;
    SSHOperation commandOp;
    commandOp.type = WorkingDirectoryTest;
    commandOp.adminCommand = "[ -d " + m_nextWorkingDir +" ] && echo 'exists'";
    m_operationsToExecute.append(commandOp);
}

void SSHSocket::m_emitError(SSHSocketError err)
{
   //disconnectFromHost();
    error((SSHSocketError)err);
}

bool SSHSocket::isConnected()
{
    return m_session !=NULL;
}


QString SSHSocket::user(){return m_user;}
QString SSHSocket::host(){return m_host;}
int SSHSocket::port(){return m_port;}
