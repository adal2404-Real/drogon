/**
 *
 *  @file DrTemplateBase.cc
 *  @author An Tao
 *
 *  Copyright 2018, An Tao.  All rights reserved.
 *  https://github.com/an-tao/drogon
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Drogon
 *
 */

#include <drogon/DrClassMap.h>
#include <drogon/DrTemplateBase.h>
#include <trantor/utils/Logger.h>
#include <memory>
#include <regex>

using namespace drogon;

std::shared_ptr<DrTemplateBase> DrTemplateBase::newTemplate(
    const std::string &templateName)
{
    LOG_TRACE << "http view name=" << templateName;
    auto l = templateName.length();
    std::string nameWithoutExt = templateName;
    if (l >= 4 && templateName[l - 4] == '.' && templateName[l - 3] == 'c' &&
        templateName[l - 2] == 's' && templateName[l - 1] == 'p')
    {
        nameWithoutExt = templateName.substr(0, l - 4);
    }

    std::string::size_type pos = 0;
    if (nameWithoutExt.size() > 0 &&
        (nameWithoutExt[0] == '/' || nameWithoutExt[0] == '\\'))
    {
        pos = 1;
    }
    else if (nameWithoutExt.size() > 1 && nameWithoutExt[0] == '.' &&
             (nameWithoutExt[1] == '/' || nameWithoutExt[1] == '\\'))
    {
        pos = 2;
    }

    std::string namespaceName;
    namespaceName.reserve(nameWithoutExt.size());
    for (size_t i = pos; i < nameWithoutExt.length(); ++i)
    {
        if (nameWithoutExt[i] == '/' || nameWithoutExt[i] == '\\')
        {
            namespaceName.append("::");
        }
        else
        {
            namespaceName.push_back(nameWithoutExt[i]);
        }
    }

    auto obj = drogon::DrClassMap::newObject(namespaceName);
    if (obj)
    {
        return std::shared_ptr<DrTemplateBase>(
            dynamic_cast<DrTemplateBase *>(obj));
    }

    std::string underscoreName;
    underscoreName.reserve(nameWithoutExt.size());
    for (size_t i = pos; i < nameWithoutExt.length(); ++i)
    {
        if (nameWithoutExt[i] == '/' || nameWithoutExt[i] == '\\')
        {
            underscoreName.push_back('_');
        }
        else
        {
            underscoreName.push_back(nameWithoutExt[i]);
        }
    }

    if (underscoreName != namespaceName)
    {
        obj = drogon::DrClassMap::newObject(underscoreName);
        if (obj)
        {
            return std::shared_ptr<DrTemplateBase>(
                dynamic_cast<DrTemplateBase *>(obj));
        }
    }

    return std::shared_ptr<DrTemplateBase>(dynamic_cast<DrTemplateBase *>(
        drogon::DrClassMap::newObject(templateName)));
}
